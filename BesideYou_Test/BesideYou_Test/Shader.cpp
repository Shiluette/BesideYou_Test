#include "stdafx.h"
//#include "Shader.h"

//월드 변환 행렬을 위한 상수 버퍼는 쉐이더 객체의 정적(static) 데이터 멤버이다.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;

CShader::CShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
}

void CShader::BuildObjects(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	//파일 “Effect.fx”에서 정점-쉐이더의 시작 함수가 "VS"인 정점-쉐이더를 생성한다. 
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	//파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

	//0720 에서 아마 없어도 되지 않을까싶다. 임의로 지웠음
	CreateShaderVariables(pd3dDevice);
}

void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

CSceneShader::CSceneShader()
{
}

CSceneShader::~CSceneShader()
{
}

void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	CCubeMesh *pCubeMesh[2];
	pCubeMesh[0] = new CCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f, D3DCOLOR_XRGB(0, 128, 0));
	pCubeMesh[1] = new CCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f, D3DCOLOR_XRGB(128, 0, 0));

	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects];

	CRotatingObject *pRotatingObject = NULL;
	
	for (int i = 0; i < m_nObjects; i++)
	{
		pRotatingObject = new CRotatingObject();
		pRotatingObject->SetMesh(pCubeMesh[i]);
		pRotatingObject->SetPosition(i * 30, 5, 0);
		pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		pRotatingObject->SetRotationSpeed(10.0f);
		m_ppObjects[i] = pRotatingObject;
	}
}

CPlayerShader::CPlayerShader()
{
	m_pMaterial = NULL;
}

CPlayerShader::~CPlayerShader()
{
	if (m_pMaterial) m_pMaterial->Release();
}

void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	//비정적멤버 참조는 특정개체에 상대적이어야합니다는 CPlayerShader가 해당 클래스를 상속되지않았을때 오류로 뜬다.

	//보통
	//CShader::CreateShader(pd3dDevice);

	//2.25-1
	//조명만 썻을때
	//CIlluminatedShader::CreateShader(pd3dDevice);

	//2.26
	//텍스처만 썻을때
	CTexturedShader::CreateShader(pd3dDevice);
}

void CPlayerShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//2.26
	//텍스쳐 맵핑에 사용할 샘플러 상태 객체를 생성한다.
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
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	//2.26
	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	CTexture *pStoneTexture = new CTexture(1, 1, 0, 0);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Miscellaneous/Stone01.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
	pStoneTexture->SetTexture(0, pd3dsrvTexture);
	pStoneTexture->SetSampler(0, pd3dSamplerState);
	pd3dsrvTexture->Release();

	//2.26
	m_pTexture = pStoneTexture;
	if (pStoneTexture) pStoneTexture->AddRef();

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	ID3D11DeviceContext *pd3dDeviceContext = NULL;
	pd3dDevice->GetImmediateContext(&pd3dDeviceContext);

	{
		//2.25
		CMaterial *pRedMaterial = new CMaterial();
		pRedMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pRedMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pRedMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
		pRedMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

		CCubeMeshTextured *pMesh = new CCubeMeshTextured(pd3dDevice, 10.0f, 10.0f, 10.0f);
		CTerrainPlayer *pPlayer = new CTerrainPlayer(1);
		pPlayer->SetMesh(pMesh);
		pPlayer->CreateShaderVariables(pd3dDevice);
		pPlayer->ChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);
		pPlayer->SetMaterial(pRedMaterial);
		pPlayer->SetTexture(m_pTexture);
		

		CCamera *pCamera = pPlayer->GetCamera();
		pCamera->SetViewport(pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		pCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
		pCamera->GenerateViewMatrix();

		//pAirplanePlayer->SetPosition(D3DXVECTOR3(0.0f, 10.0f, -50.0f));
		m_ppObjects[0] = pPlayer;
	}


	//{
	//	////2.19
	//	CCubeMesh *pCubeMesh = new CCubeMesh(pd3dDevice, 10.0f, 10.0f, 10.0f, D3DXCOLOR(0.5f, 1.0f, 0.0f, 0.0f));
	//	CTerrainPlayer *pTerrainPlayer = new CTerrainPlayer(1);
	//	pTerrainPlayer->SetMesh(pCubeMesh);
	//	pTerrainPlayer->CreateShaderVariables(pd3dDevice);
	//	pTerrainPlayer->ChangeCamera(pd3dDevice, SPACESHIP_CAMERA, 0.0f);

	//	//2.22
	//	CCamera *pTestCamera = pTerrainPlayer->GetCamera();
	//	pTestCamera->SetViewport(pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	//	pTestCamera->GenerateProjectionMatrix(1.01f, 50000.0f, ASPECT_RATIO, 60.0f);
	//	pTestCamera->GenerateViewMatrix();
	//	//2.22

	//	pTerrainPlayer->SetPosition(D3DXVECTOR3(0.0f, 10.0f, -50.0f));
	//	m_ppObjects[1] = pTerrainPlayer;
	//	////2.19
	//}

	if (pd3dDeviceContext) pd3dDeviceContext->Release();
}

void CPlayerShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//3인칭 카메라일 때 플레이어를 렌더링한다.
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		CShader::Render(pd3dDeviceContext, pCamera);
	}
}

CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	//{
	//	//2.25-1
	//	//지형을 위한 재질을 생성한다. 재질은 녹색을 많이 반사한다.
	//	CMaterial *pTerrainMaterial = new CMaterial();
	//	pTerrainMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.9f, 0.2f, 1.0f);
	//	pTerrainMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.0f, 0.2f, 0.0f, 1.0f);
	//	pTerrainMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	//	pTerrainMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	//	D3DXVECTOR3 d3dxvScale(8.0f, 3.0f, 8.0f);
	//	D3DXCOLOR d3dxColor(0.0f, 0.2f, 0.1f, 0.0f);

	//	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, 257, 257, 257, 257, _T("../Data/Terrain/BesideYouHeightMap5.raw"), d3dxvScale, d3dxColor);
	//	//m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, 257, 257, 257, 257, _T("../Data/Terrain/HeightMap.raw"), d3dxvScale, d3dxColor);

	//	//2.25-1
	//	m_ppObjects[0]->SetMaterial(pTerrainMaterial);
	//}

	//2.27
	{
		ID3D11SamplerState *pd3dSamplerState = NULL;
		D3D11_SAMPLER_DESC d3dSamplerDesc;
		ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		d3dSamplerDesc.MinLOD = 0;
		d3dSamplerDesc.MaxLOD = 0;

		pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

		CTexture *pTerrainTexture = new CTexture(1, 1, 0, 0);
		ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
		D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Data/Terrain/ground1.jpg"), NULL, NULL, &pd3dsrvTexture, NULL);
		pTerrainTexture->SetTexture(0, pd3dsrvTexture);
		pTerrainTexture->SetSampler(0, pd3dSamplerState);
		pd3dsrvTexture->Release();
		pd3dSamplerState->Release();

		D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
		D3DXCOLOR d3dxColor(0.0f, 0.2f, 0.1f, 0.0f);
		m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, 257, 257, 257, 257, _T("../Data/Terrain/BesideYouHeightMap5.raw"), d3dxvScale, d3dxColor);
		m_ppObjects[0]->SetTexture(pTerrainTexture);
	}

	////2.27-1
	//{
	//	//지형은 텍스쳐가 2개이므로 2개의 샘플러 객체가 필요하다. 
	//	ID3D11SamplerState *pd3dBaseSamplerState = NULL;
	//	D3D11_SAMPLER_DESC d3dSamplerDesc;
	//	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	//	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	//	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	//	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	//	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//	d3dSamplerDesc.MinLOD = 0;
	//	d3dSamplerDesc.MaxLOD = 0;
	//	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

	//	ID3D11SamplerState *pd3dDetailSamplerState = NULL;
	//	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);

	//	CTexture *pTerrainTexture = new CTexture(2, 2, 0, 0);
	//	ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
	//	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Base_Texture.jpg"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
	//	pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture);
	//	pTerrainTexture->SetSampler(0, pd3dBaseSamplerState);
	//	pd3dsrvBaseTexture->Release();
	//	pd3dBaseSamplerState->Release();

	//	ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
	//	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Detail_Texture_7.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
	//	pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture);
	//	pTerrainTexture->SetSampler(1, pd3dDetailSamplerState);
	//	pd3dsrvDetailTexture->Release();
	//	pd3dDetailSamplerState->Release();

	//	D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
	//	D3DXCOLOR d3dxColor(0.0f, 0.2f, 0.1f, 0.0f);
	//	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, 257, 257, 257, 257, _T("../Data/Terrain/BesideYouHeightMap5.raw"), d3dxvScale, d3dxColor);
	//	m_ppObjects[0]->SetTexture(pTerrainTexture);
	//}

	//2.27-2
	{
		//지형은 텍스쳐가 2개이므로 2개의 샘플러 객체가 필요하다. 
		ID3D11SamplerState *pd3dBaseSamplerState = NULL;
		D3D11_SAMPLER_DESC d3dSamplerDesc;
		ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		d3dSamplerDesc.MinLOD = 0;
		d3dSamplerDesc.MaxLOD = 0;
		pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dBaseSamplerState);

		ID3D11SamplerState *pd3dDetailSamplerState = NULL;
		d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dDetailSamplerState);

		CTexture *pTerrainTexture = new CTexture(2, 2, 0, 0);
		ID3D11ShaderResourceView *pd3dsrvBaseTexture = NULL;
		D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Base_Texture.jpg"), NULL, NULL, &pd3dsrvBaseTexture, NULL);
		pTerrainTexture->SetTexture(0, pd3dsrvBaseTexture);
		pTerrainTexture->SetSampler(0, pd3dBaseSamplerState);
		pd3dsrvBaseTexture->Release();
		pd3dBaseSamplerState->Release();

		ID3D11ShaderResourceView *pd3dsrvDetailTexture = NULL;
		D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Assets/Image/Terrain/Detail_Texture_7.jpg"), NULL, NULL, &pd3dsrvDetailTexture, NULL);
		pTerrainTexture->SetTexture(1, pd3dsrvDetailTexture);
		pTerrainTexture->SetSampler(1, pd3dDetailSamplerState);
		pd3dsrvDetailTexture->Release();
		pd3dDetailSamplerState->Release();

		CMaterial *pTerrainMaterial = new CMaterial();
		pTerrainMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.8f, 1.0f, 0.2f, 1.0f);
		pTerrainMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.3f, 0.1f, 1.0f);
		pTerrainMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
		pTerrainMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

		D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
		D3DXCOLOR d3dxColor(0.0f, 0.2f, 0.1f, 0.0f);
		m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, 257, 257, 257, 257, _T("../Data/Terrain/BesideYouHeightMap5.raw"), d3dxvScale, d3dxColor);
		m_ppObjects[0]->SetTexture(pTerrainTexture);
		m_ppObjects[0]->SetMaterial(pTerrainMaterial);
	}

	//터레인의 초기위치
	//m_ppObjects[0]->SetPosition(-1250, 0, -1250);
}

//2.23
CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return ((CHeightMapTerrain*)m_ppObjects[0]);
}

//2.25
ID3D11Buffer *CIlluminatedShader::m_pd3dcbMaterial = NULL;

//2.25
CIlluminatedShader::CIlluminatedShader()
{
}

//2.25
CIlluminatedShader::~CIlluminatedShader()
{
}

//2.25
void CIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

//2.25
void CIlluminatedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}

//2.25
void CIlluminatedShader::ReleaseShaderVariables()
{
	if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release();
}

//2.25
void CIlluminatedShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}

//2.26
CTexturedShader::CTexturedShader()
{
}

//2.26
CTexturedShader::~CTexturedShader()
{
}

//2.26
void CTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}

//2.27-1
CDetailTexturedShader::CDetailTexturedShader()
{
}

//2.27-1
CDetailTexturedShader::~CDetailTexturedShader()
{
}

//2.27-1
void CDetailTexturedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDetailTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDetailTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}

//2.27-2
CTexturedIlluminatedShader::CTexturedIlluminatedShader()
{
}

//2.27-2
CTexturedIlluminatedShader::~CTexturedIlluminatedShader()
{
}

//2.27-2
void CTexturedIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

//2.27-2
CDetailTexturedIlluminatedShader::CDetailTexturedIlluminatedShader()
{
}

//2.27-2
CDetailTexturedIlluminatedShader::~CDetailTexturedIlluminatedShader()
{
}

//2.27-2
void CDetailTexturedIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDetailTexturedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDetailTexturedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}