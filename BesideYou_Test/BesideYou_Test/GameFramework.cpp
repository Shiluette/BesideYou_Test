#include "stdafx.h"
//#include "GameFramework.h"

//백업되는지 확인해볼까?

//3.2
CTexture* CreateTexture(ID3D11Device* pd3dDevice, WCHAR* ptrstring, ID3D11ShaderResourceView** pd3dsrvTexture, ID3D11SamplerState** pd3dSamplerState, int nTextureStartSlot, int nSamplerStartSlot)
{
	CTexture* pTexture = new CTexture(1, 1, nTextureStartSlot, nSamplerStartSlot);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, ptrstring, NULL, NULL, pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, *pd3dsrvTexture);
	pTexture->SetSampler(0, *pd3dSamplerState);
	(*pd3dsrvTexture)->Release();
	return pTexture;
}

CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("BesideYou_ ("));

	m_pPlayer = NULL;

	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;

	m_pPlayerShader = NULL;
}

CGameFramework::~CGameFramework()
{
}

//다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	if (!CreateDirect3DDisplay()) return(false);

	//렌더링할 객체(게임 월드 객체)를 생성한다. 
	BuildObjects();

	//다이렉트2d
	Build2DObjects();

	return(true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	//렌더 타겟과 같은 크기의 깊이 버퍼(Depth Buffer)를 생성한다.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	//생성한 깊이 버퍼(Depth Buffer)에 대한 뷰를 생성한다.
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	ZeroMemory(&d3dViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	//다이렉트2
	float fdpiX, fdpiY;
	m_pd2dFactory->GetDesktopDpi(&fdpiX, &fdpiY);
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			, fdpiX
			, fdpiY
			);
	IDXGISurface2 *dxgiBackBuffer;
	ID2D1Bitmap1 *pd2dBitmapBackbuffer;
	hResult = m_pDXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	hResult = m_pd2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &pd2dBitmapBackbuffer);
	m_pd2dContext->SetTarget(pd2dBitmapBackbuffer);
	dxgiBackBuffer->Release();
	pd2dBitmapBackbuffer->Release();

	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*0*/;

	UINT dwCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL pd3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(pd3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hResult = S_OK;
#ifdef _WITH_DEVICE_AND_SWAPCHAIN
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
#else
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDevice(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, pd3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pd3dDevice) return(false);

	if (FAILED(hResult = m_pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_n4xMSAAQualities))) return(false);
#ifdef _WITH_MSAA4_MULTISAMPLING
	dxgiSwapChainDesc.SampleDesc.Count = 4;
	dxgiSwapChainDesc.SampleDesc.Quality = m_n4xMSAAQualities - 1;
#else
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
#endif
	IDXGIFactory1 *pdxgiFactory = NULL;
	if (FAILED(hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&pdxgiFactory))) return(false);
	IDXGIDevice2 *pdxgiDevice = NULL;
	if (FAILED(hResult = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice2), (void **)&pdxgiDevice))) return(false);
	if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice, &dxgiSwapChainDesc, &m_pDXGISwapChain))) return(false);

	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, reinterpret_cast<LPVOID*>(&m_pd2dFactory));
	hResult = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&m_pdwFactory);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	hResult = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pwicFactory));
	hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	hResult = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dContext);

	if (pdxgiDevice) pdxgiDevice->Release();
	if (pdxgiFactory) pdxgiFactory->Release();
#endif

	if (!CreateRenderTargetDepthStencilView()) return(false);

	return(true);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
			/*‘F1’ 키를 누르면 1인칭 카메라, ‘F2’ 키를 누르면 스페이스-쉽 카메라로 변경한다, ‘F3’ 키를 누르면 3인칭 카메라로 변경한다.*/
		case VK_F1:
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pPlayer->ChangeCamera(m_pd3dDevice, (wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;

		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//3.11 인자로 m_pPlayerShader추가
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, m_pPlayerShader);

	switch (nMessageID)
	{
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();

		if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
		if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

		m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencilView();
		
		CCamera *pCamera = m_pPlayer->GetCamera();
		if (pCamera) pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

//다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라. 
void CGameFramework::OnDestroy()
{
	//게임 객체를 소멸한다. 
	ReleaseObjects();

	//Direct3D와 관련된 객체를 소멸한다. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();

	//다이렉트2
	if (m_pd2dFactory) m_pd2dFactory->Release();
	if (m_pd2dDevice) m_pd2dDevice->Release();
	if (m_pd2dContext) m_pd2dContext->Release();
	if (m_pdwFactory) m_pdwFactory->Release();
	if (m_pwicFactory) m_pwicFactory->Release();

	//2dObject
	if (m_dwExplainFormat) m_dwExplainFormat->Release();
	if (m_dwMyChattingFormat) m_dwMyChattingFormat->Release();
	if (m_pd2drcBox) m_pd2drcBox->Release();
	if (m_pd2dsbrBeige) m_pd2dsbrBeige->Release();
	if (m_pd2dsbrGreenColor) m_pd2dsbrGreenColor->Release();
	if (m_pd2dsbrRedColor) m_pd2dsbrRedColor->Release();
}

void CGameFramework::BuildObjects()
{
	//CShader 클래스의 정적(static) 멤버 변수로 선언된 상수 버퍼를 생성한다.
	CShader::CreateShaderVariables(m_pd3dDevice);

	//2.25
	CIlluminatedShader::CreateShaderVariables(m_pd3dDevice);

	//3.2
	FBXModelDataLoad();

	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice);

	m_pPlayerShader = new CPlayerShader();
	m_pPlayerShader->CreateShader(m_pd3dDevice);
	//m_pPlayerShader->BuildObjects(m_pd3dDevice);
	//3.2
	m_pPlayerShader->BuildObjects(m_pd3dDevice, m_vtCharacterDatas);

	m_pPlayer = m_pPlayerShader->GetPlayer();

	CHeightMapTerrain * pTerrain = m_pScene->GetTerrain();
	m_pPlayer->SetPosition(D3DXVECTOR3(pTerrain->GetWidth() * 0.5f, pTerrain->GetPeakHeight() + 1000.0f, pTerrain->GetLength() * 0.5f));
	//m_pPlayer->SetPosition(D3DXVECTOR3(0, pTerrain->GetPeakHeight() + 1000.0f, 0));
	//플레이어의 위치가 변경될 때 지형의 정보에 따라 플레이어의 위치를 변경할 수 있도록 설정한다.
	m_pPlayer->SetPlayerUpdatedContext(pTerrain);
	//카메라의 위치가 변경될 때 지형의 정보에 따라 카메라의 위치를 변경할 수 있도록 설정한다.
	m_pPlayer->SetCameraUpdatedContext(pTerrain);

	//이거 왜 있는거지?
	m_pCamera = m_pPlayer->GetCamera();
	m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->GenerateViewMatrix();

	m_pScene->SetCamera(m_pCamera);
}

void CGameFramework::ReleaseObjects()
{
	//CShader 클래스의 정적(static) 멤버 변수로 선언된 상수 버퍼를 반환한다.
	CShader::ReleaseShaderVariables();

	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_pPlayerShader) m_pPlayerShader->ReleaseObjects();
	if (m_pPlayerShader) delete m_pPlayerShader;
}

//다이렉트2d
void CGameFramework::Build2DObjects()
{
	m_pdwFactory->CreateTextFormat(L"맑은고딕", nullptr, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_MEDIUM, 15.0f, L"ko-ko", &m_dwExplainFormat);
	//m_pdwFactory->CreateTextFormat(L"맑은고딕", nullptr, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"ko-ko", &m_dwMyChattingFormat);
	HRESULT result;

	result = m_pd2dFactory->CreateRectangleGeometry(D2D1::RectF(0, 0, 350, 450), &m_pd2drcBox);		// rectf는 상자크기 지정
	result = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Beige, 0.5f), &m_pd2dsbrBeige);
	result = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green, 1.0f), &m_pd2dsbrGreenColor);
	result = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &m_pd2dsbrRedColor);

	//bool b_result = false;
	m_nBitmaps = 1;
	m_ppd2dBitmap = new ID2D1Bitmap1*[m_nBitmaps];
	result = LoadImageFromFile(L"Brick01.jpg", &m_ppd2dBitmap[0], nullptr, 0, 0, WICBitmapTransformRotate90);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_on.jpg", &m_ppd2dBitmap[BITMAP::LobbyStartOff], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_off.jpg", &m_ppd2dBitmap[BITMAP::LobbyStartOn], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_off.jpg", &m_ppd2dBitmap[BITMAP::LobbySelectOff], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_off.jpg", &m_ppd2dBitmap[BITMAP::LobbySelectOn], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_off.jpg", &m_ppd2dBitmap[BITMAP::LobbyExitOff], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/button_off.jpg", &m_ppd2dBitmap[BITMAP::LobbyExitOn], nullptr, 0, 0, WICBitmapTransformRotate0);

	//b_result = LoadImageFromFile(L"Image/Bitmap/RightArrow.png", &m_ppd2dBitmap[BITMAP::SelectArrowLeftOff], nullptr, 0, 0, WICBitmapTransformRotate180);
	//b_result = LoadImageFromFile(L"Image/Bitmap/RightArrowOn.png", &m_ppd2dBitmap[BITMAP::SelectArrowLeftOn], nullptr, 0, 0, WICBitmapTransformRotate180);
	//b_result = LoadImageFromFile(L"Image/Bitmap/RightArrow.png", &m_ppd2dBitmap[BITMAP::SelectArrowRightOff], nullptr, 0, 0, WICBitmapTransformRotate0);
	//b_result = LoadImageFromFile(L"Image/Bitmap/RightArrowOn.png", &m_ppd2dBitmap[BITMAP::SelectArrowRightOn], nullptr, 0, 0, WICBitmapTransformRotate0);
}

void CGameFramework::Render2D()
{
	m_pd2dContext->BeginDraw();

	D2D1::Matrix3x2F mtx = D2D1::Matrix3x2F::Identity();
	m_pd2dContext->SetTransform(mtx);

	D2D1::Matrix3x2F rot = D2D1::Matrix3x2F::Identity();

	//베이지 사각형 그리기
	//mtx = D2D1::Matrix3x2F::Translation(0, 0);
	//m_pd2dContext->SetTransform(mtx);
	//m_pd2dContext->FillGeometry(m_pd2drcBox, m_pd2dsbrBeige);

	//2d텍스쳐 띄우기
	//m_pd2dContext->DrawBitmap(m_ppd2dBitmap[0], D2D1::RectF(0.f, 0.f, m_nWndClientWidth / 3, m_nWndClientHeight / 3), 0.9f);

	//글씨 회전시키려고 해봤음
	/*rot = D2D1::Matrix3x2F::Rotation(45.0f, D2D1::Point2F(10, 10));
	rot =  mtx * rot;
	m_pd2dContext->SetTransform(rot);*/

	//mtx = D2D1::Matrix3x2F::Translation(10, 10);
	//m_pd2dContext->SetTransform(mtx);

	mtx = D2D1::Matrix3x2F::Translation(0, 0);
	m_pd2dContext->SetTransform(mtx);

	//글씨띄우기
	wchar_t w_str[50];
	wmemset(w_str, 0, 50);
	wsprintf(w_str, L"GreenCube : (%d %d %d)", (int)m_pScene->ReturnCubePosition(0).x , (int)m_pScene->ReturnCubePosition(0).y , (int)m_pScene->ReturnCubePosition(0).z);
	//m_GameTimer.GetFrameRate();
	m_pd2dContext->DrawTextW(w_str, 50, m_dwExplainFormat, D2D1::RectF(0.0f, 0.0f, 600.0f, 300.0f), m_pd2dsbrGreenColor);

	m_pScene->ReturnCubePosition(0);
	mtx = D2D1::Matrix3x2F::Translation(0, 20);
	m_pd2dContext->SetTransform(mtx);

	wchar_t w_str2[50];
	wmemset(w_str2, 0, 50);
	wsprintf(w_str2, L"RedCube : (%d %d %d)", (int)m_pScene->ReturnCubePosition(1).x, (int)m_pScene->ReturnCubePosition(1).y, (int)m_pScene->ReturnCubePosition(1).z);
	m_pd2dContext->DrawTextW(w_str2, 50, m_dwExplainFormat, D2D1::RectF(0.0f, 0.0f, 600.0f, 300.0f), m_pd2dsbrRedColor);

	m_pd2dContext->EndDraw();
}

void CGameFramework::Release2DObjects() {
	if (m_pd2dsbrBeige) m_pd2dsbrBeige->Release();
	if (m_pd2drcBox) m_pd2drcBox->Release();
	if (m_pd2dsbrGreenColor) m_pd2dsbrGreenColor->Release();
	if (m_pd2dsbrRedColor) m_pd2dsbrRedColor->Release();
	if (m_dwExplainFormat) m_dwExplainFormat->Release();

	if (m_ppd2dBitmap) {
		for (auto i = 0; i < m_nBitmaps; ++i)
			m_ppd2dBitmap[i]->Release();
	}
	delete[]m_ppd2dBitmap;
}

bool CGameFramework::LoadImageFromFile(_TCHAR *pszstrFileName, ID2D1Bitmap1 **ppd2dBitmap, D2D_RECT_U *pd2drcImage, UINT nWidth, UINT nHeight, WICBitmapTransformOptions nFlipRotation) {
	HRESULT hResult;
	IWICBitmapDecoder *pwicBitmapDecoder = NULL;
	if (FAILED(hResult = m_pwicFactory->CreateDecoderFromFilename(pszstrFileName, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder))) return(false);
	IWICBitmapFrameDecode *pwicBitmapFrameDecode = NULL;
	if (FAILED(hResult = pwicBitmapDecoder->GetFrame(0, &pwicBitmapFrameDecode))) return(false);
	IWICBitmapSource *pwicSource = pwicBitmapFrameDecode;
	UINT nImageWidth, nImageHeight;
	if (FAILED(hResult = pwicSource->GetSize(&nImageWidth, &nImageHeight))) return(false);
	IWICFormatConverter   *pwicFormatConverter = NULL;
	IWICBitmapScaler *pwicScaler = NULL;
	IWICBitmapClipper *pwicClipper = NULL;
	IWICBitmapFlipRotator *pwicFlipRotator = NULL;
	if (pd2drcImage)
	{
		if (pd2drcImage->left < 0) pd2drcImage->left = 0;
		if (pd2drcImage->top < 0) pd2drcImage->top = 0;
		if (pd2drcImage->right > nImageWidth) pd2drcImage->right = nImageWidth;
		if (pd2drcImage->bottom > nImageHeight) pd2drcImage->bottom = nImageHeight;
		WICRect wicRect = { pd2drcImage->left, pd2drcImage->top, (pd2drcImage->right - pd2drcImage->left), (pd2drcImage->bottom - pd2drcImage->top) };
		if (FAILED(hResult = m_pwicFactory->CreateBitmapClipper(&pwicClipper))) return(false);
		if (FAILED(hResult = pwicClipper->Initialize(pwicSource, &wicRect))) return(false);
		pwicSource = pwicClipper;
	}
	if ((nWidth != 0) || (nHeight != 0))
	{
		if (nWidth == 0) nWidth = UINT(float(nHeight) / float(nImageHeight) * float(nImageWidth));
		if (nHeight == 0) nHeight = UINT(float(nWidth) / float(nImageWidth) * float(nImageHeight));
		if (FAILED(hResult = m_pwicFactory->CreateBitmapScaler(&pwicScaler))) return(false);
		if (FAILED(hResult = pwicScaler->Initialize(pwicSource, nWidth, nHeight, WICBitmapInterpolationModeCubic))) return(false);
		pwicSource = pwicScaler;
	}
	if (nFlipRotation != WICBitmapTransformRotate0)
	{
		if (FAILED(hResult = m_pwicFactory->CreateBitmapFlipRotator(&pwicFlipRotator))) return(false);
		if (FAILED(hResult = pwicFlipRotator->Initialize(pwicSource, nFlipRotation))) return(false);
		pwicSource = pwicFlipRotator;
	}
	if (FAILED(hResult = m_pwicFactory->CreateFormatConverter(&pwicFormatConverter))) return(false);
	if (FAILED(hResult = pwicFormatConverter->Initialize(pwicSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut))) return(false);
	if (FAILED(hResult = m_pd2dContext->CreateBitmapFromWicBitmap(pwicFormatConverter, NULL, ppd2dBitmap))) return(false);

	if (pwicBitmapFrameDecode) pwicBitmapFrameDecode->Release();
	if (pwicBitmapDecoder) pwicBitmapDecoder->Release();
	if (pwicFormatConverter) pwicFormatConverter->Release();
	if (pwicClipper) pwicClipper->Release();
	if (pwicScaler) pwicScaler->Release();
	if (pwicFlipRotator) pwicFlipRotator->Release();

	return(true);
}

void CGameFramework::ProcessInput()
{
	bool bProcessedByScene = false;
	if (m_pScene) bProcessedByScene = m_pScene->ProcessInput();
	if (!bProcessedByScene)
	{
		static UCHAR pKeyBuffer[256];
		DWORD dwDirection = 0;
		/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[VK_UP] & 0xF0)dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		}
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의 메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된 것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스를 좌우 또는 상하로 움직이면 플레이어를 x-축 또는 y-축으로 회전한다.*/
		if (GetCapture() == m_hWnd)
		{
			//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
			SetCursor(NULL);
			//현재 마우스 커서의 위치를 가져온다.
			GetCursorPos(&ptCursorPos);
			//마우스 버튼이 눌린 채로 이전 위치에서 현재 마우스 커서의 위치까지 움직인 양을 구한다.
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
		//플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다. 오른쪽 마우스 버튼이 눌려진 경우 cxDelta는 z-축의 회전을 나타낸다.*/
				if (pKeyBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			/*플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다). 이동 거리는 시간에 비례하도록 한다. 플레이어의 이동 속력은 (50/초)로 가정한다. 만약 플레이어의 이동 속력이 있다면 그 값을 사용한다.*/
			if (dwDirection) m_pPlayer->Move(dwDirection, 100.0f * m_GameTimer.GetTimeElapsed(), true);
		}

		//if (GetKeyState('D') & 0xFF00) m_pScene->ProcessInput();
	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다.
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());

	//3.6
	//if (m_pPlayer) m_pPlayer->Animate(m_GameTimer.GetTimeElapsed());
	//if (m_pPlayerShader) m_pPlayerShader->AnimateObjects(m_GameTimer.GetTimeElapsed());

	//3.11
	m_pPlayerShader->GetFBXMesh->FBXFrameAdvance(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. 
	m_GameTimer.Tick();

	ProcessInput();

	AnimateObjects();

	float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	//렌더 타겟 뷰를 색상(RGB: 0.0f, 0.125f, 0.3f)으로 지운다. 
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (m_pPlayer) m_pPlayer->UpdateShaderVariables(m_pd3dDeviceContext);

	if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, m_pCamera);

	//다이렉트2d
	Render2D();

	/*렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지운다. 이제 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.*/
	//m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//3인칭 카메라일 때 플레이어를 렌더링한다.
	if (m_pPlayerShader) {
		//3.11
		m_pPlayerShader->GetFBXMesh->UpdateBoneTransform(m_pd3dDeviceContext, m_pPlayerShader->GetFBXMesh->GetFBXAnimationNum(), m_pPlayerShader->GetFBXMesh->GetFBXNowFrameNum());
		m_pPlayerShader->Render(m_pd3dDeviceContext, m_pCamera);
	}

	//후면버퍼를 전면버퍼로 프리젠트한다. 
	m_pDXGISwapChain->Present(0, 0);

	/*현재의 프레임 레이트를 문자열로 가져와서 주 윈도우의 타이틀로 출력한다. m_pszBuffer 문자열이 "LapProject ("으로 초기화되었으므로 (m_pszBuffer+12)에서부터 프레임 레이트를 문자열로 출력하여 “ FPS)” 문자열과 합친다.
	_itow_s(m_nCurrentFrameRate, (m_pszBuffer+12), 37, 10);
	wcscat_s((m_pszBuffer+12), 37, _T(" FPS)"));
	*/
	m_GameTimer.GetFrameRate(m_pszBuffer + 12, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);
}

//3.2
void CGameFramework::FBXModelDataLoad(void)
{
	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	m_pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	CTexture *TestTexture = CreateTexture(m_pd3dDevice, _T("../Data/BesideYouData/Vehice_Ambulance.png"), &pd3dsrvTexture, &pd3dSamplerState, 0, 0);
	pd3dSamplerState->Release();

	//CMesh* pTestMesh = new CFBXMesh(m_pd3dDevice, "../Data/ambo_mesh.data", 0.01);

	CMesh * pTestMesh = new CAssetMesh(m_pd3dDevice, "../Data/BesideYouData/ambo_mesh.data");
	
	m_vtCharacterDatas.push_back(new ModelContainer{ "TestModel", pTestMesh, TestTexture });

	for (auto& iter : m_vtCharacterDatas)
		iter->AddRef();
}