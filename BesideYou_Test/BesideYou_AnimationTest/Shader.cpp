#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "SKinnedMesh.h"

//월드 변환 행렬을 위한 상수 버퍼는 쉐이더 객체의 정적(static) 데이터 멤버이다.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;

BYTE *ReadCompiledEffectFile(WCHAR *pszFileName, int *pnReadBytes)
{
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	*pnReadBytes = ::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
	return(pByteCode);
}

ID3D11Buffer *CIlluminatedShader::m_pd3dcbMaterial = NULL;

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

//이 함수는 쉐이더 파일에서 정점-쉐이더와 입력-레이아웃를 생성한다. 
void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, 
	LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, 
	D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, 
		dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, 
			pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

//이 함수는 쉐이더 파일에서 픽셀 - 쉐이더를 생성한다.
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


//CShader 클래스의 CreateShader() 멤버 함수와 Render() 멤버 함수를 다음과 같이 정의한다.
void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	/*IA 단계에 설정할 입력-레이아웃을 정의한다.
	정점 버퍼의 한 원소가 CVertex 클래스의 멤버 변수(D3DXVECTOR3 즉, 실수 세 개)이므로
	입력-레이아웃은 실수(32-비트) 3개로 구성되며 시멘틱이 “POSITION”이고 정점 데이터임을 표현한다.*/

	//하나의 정점은 위치 벡터(실수 3개)와 색상(실수 4개)을 포함한다. 
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		/*LPCSTR SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;*/
	};

	UINT nElements = ARRAYSIZE(d3dInputLayout);
	
	////파일 “Effect.fx”에서 정점-쉐이더의 시작 함수가 "VS"인 정점-쉐이더를 생성한다. 
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	////파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PS", "ps_5_0", &m_pd3dPixelShader);

	//컴파일된 쉐이더 코드의 이름이 VS.fxo와 PS.fxo이다.

	CreateShaderVariables(pd3dDevice);
}

//쉐이더 렌더링 함수를 정의한다.
void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//쉐이더 객체의 모드 게임 객체들을 렌더링한다.
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext,pCamera);
			}
		}
	}
/*격자 메쉬의 크기가 (257x257)일 때와 (17x17)일 때의 프레임 레이트를 비교하라.
포토샵과 같은 프로그램을 사용하여 이미지의 크기가 
(1025x1025)인 높이 맵을 생성하여 격자 메쉬의 크기가
(1025x1025), (25x25), (50x50), (100x100), (200x200),
.... 일 때의 프레임 레이트를 비교하라. */
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
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;//서브리소스?

	//여기서 상수버퍼를 지워버리고, 서브리소스를 매개로 연결, 매개로 쓰는것 같다.
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	
	//서브리소스의 포인터를 캐스팅해서 임시 행렬포인터에 넘긴다.
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	
	//현재 월드행렬의 전치행렬을 임시 행렬에 넘긴다.
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	//상수버퍼에 대한 map을 종료, unmap한다.
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
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

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	/*정육면체 객체를 x-축, y-축, z-축으로 각각 13개씩 총 2,197개(13x13x13) 생성하고 구 객체를 16개 생성한다.*/   
		m_nObjects = (xObjects * 2 + 1)*(yObjects * 2 + 1)*(zObjects * 2 + 1) + (8 * 2);
	m_ppObjects = new CGameObject*[m_nObjects];

	//정육면체 메쉬를 생성한다.
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, 12.0f, 12.0f, 12.0f, D3DCOLOR_XRGB(0, 0, 128));

	float fxPitch = 12.0f * 1.7f;
	float fyPitch = 12.0f * 1.7f;
	float fzPitch = 12.0f * 1.7f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	//구 메쉬를 생성한다.
	CSphereMeshDiffused *pSphereMesh = new CSphereMeshDiffused(pd3dDevice, 12.0f, 20, 20, D3DCOLOR_XRGB(64, 0, 0));
	
	/*구 객체는 공전하는 객체이다. 8개는 월드좌표계의 xy-평면에서 원점을 중심으로 45도 간격으로 위치한다. z-축으로 회전한다. 8개는 월드좌표계의 yz-평면에서 원점을 중심으로 45도 간격으로 위치한다. x-축으로 회전한다.*/
	CRevolvingObject *pRevolvingObject = NULL;
	float fRadius = (xObjects * fxPitch * 2.0f) * 1.5f;
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j), 0.0f);
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		pRevolvingObject->SetRevolutionSpeed(10.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(0.0f, fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j));
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
		pRevolvingObject->SetRevolutionSpeed(20.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
}

CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
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

CInstancingShader::CInstancingShader()
{
	m_pd3dCubeInstanceBuffer = NULL;
	m_pd3dSphereInstanceBuffer = NULL;
	m_pMaterial = NULL;
	m_pTexture = NULL;
}

CInstancingShader::~CInstancingShader()
{
	if (m_pd3dCubeInstanceBuffer) m_pd3dCubeInstanceBuffer->Release();
	if (m_pd3dSphereInstanceBuffer) m_pd3dSphereInstanceBuffer->Release();
	if (m_pMaterial) m_pMaterial->Release();
	if (m_pTexture) m_pTexture->Release();
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSInstancedTexturedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSInstancedTexturedColor", "ps_5_0", &m_pd3dPixelShader);
}

ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	/*버퍼의 초기화 데이터가 없으면 동적 버퍼로 생성한다. 즉, 나중에 매핑을 하여 내용을 채우거나 변경한다.*/
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}
void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, CHeightMapTerrain *pHeightMapTerrain, CMaterial *pMaterial, CTexture *pTexture, int k)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();
	m_pTexture = pTexture;
	if (pTexture) pTexture->AddRef();

	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	CCubeMeshTextured *pCubeMesh = new CCubeMeshTextured(pd3dDevice, 12.0f, 12.0f, 12.0f);
	CSphereMeshTextured *pSphereMesh = new CSphereMeshTextured(pd3dDevice, 12.0f, 20, 20);

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pHeightMapTerrain->GetWidth();
	float fTerrainLength = pHeightMapTerrain->GetLength();

	/*두 가지(직육면체와 구) 객체들을 지형에 일정한 간격으로 배치한다. 
	지형의 표면에 직육면체를 배치하고 직육면체 위에 구가 배치된다. 
	직육면체와 구는 빨강색, 녹색, 파랑색이 반복되도록 배치된다.*/
	int xObjects = int(fTerrainWidth / (fxPitch * 3.0f)), yObjects = 2, zObjects = int(fTerrainLength / (fzPitch * 3.0f)), i = 0;
	m_nObjects = xObjects * yObjects * zObjects;

	m_ppObjects = new CGameObject*[m_nObjects];

	D3DXVECTOR3 d3dxvRotateAxis;
	CRotatingObject *pRotatingObject = NULL;

	/*구는 3가지 종류(재질에 따라)이다. 다른 재질의 구들이 번갈아 나열되도록 한다. 
	재질의 종류에 따라 k가 0, 1, 2의 값을 가지고 k에 따라 객체의 위치를 다르게 설정한다.*/
	for (int x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			pRotatingObject = new CRotatingObject();
			pRotatingObject->SetMesh(pSphereMesh);
			float xPosition = (k * fxPitch) + (x * fxPitch * 3.0f);
			float zPosition = (k * fzPitch) + (z * fxPitch * 3.0f);
			float fHeight = pHeightMapTerrain->GetHeight(xPosition, zPosition);
			pRotatingObject->SetPosition(xPosition, fHeight + (fyPitch * 4), zPosition);
			pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			pRotatingObject->SetRotationSpeed(36.0f * (i % 10) + 36.0f);
			m_ppObjects[i++] = pRotatingObject;
		}
	}
	m_pd3dSphereInstanceBuffer = CreateInstanceBuffer(pd3dDevice, xObjects * zObjects, m_nInstanceBufferStride, NULL);
	pSphereMesh->AssembleToVertexBuffer(1, &m_pd3dSphereInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	for (int x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			pRotatingObject = new CRotatingObject();
			pRotatingObject->SetMesh(pCubeMesh);
			float xPosition = (k * fxPitch) + (x * fxPitch * 3.0f);
			float zPosition = (k * fzPitch) + (z * fxPitch * 3.0f);
			float fHeight = pHeightMapTerrain->GetHeight(xPosition, zPosition);
			pRotatingObject->SetPosition(xPosition, fHeight + 6.0f, zPosition);
			D3DXVECTOR3 d3dxvSurfaceNormal = pHeightMapTerrain->GetNormal(xPosition, zPosition);
			D3DXVec3Cross(&d3dxvRotateAxis, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvSurfaceNormal);
			float fAngle = acos(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 1.0f, 0.0f), &d3dxvSurfaceNormal));
			pRotatingObject->Rotate(&d3dxvRotateAxis, (float)D3DXToDegree(fAngle));
			pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			pRotatingObject->SetRotationSpeed(18.0f * (i % 10) + 10.0f);
			m_ppObjects[i++] = pRotatingObject;
		}
	}
	m_pd3dCubeInstanceBuffer = CreateInstanceBuffer(pd3dDevice, xObjects * zObjects, m_nInstanceBufferStride, NULL);
	pCubeMesh->AssembleToVertexBuffer(1, &m_pd3dCubeInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
}

void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pMaterial) CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);

	int nSphereObjects = m_nObjects / 2;

	int nSphereInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dSphereInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pnSphereInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < nSphereObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pnSphereInstances[nSphereInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				pnSphereInstances[nSphereInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dSphereInstanceBuffer, 0);

	CMesh *pSphereMesh = m_ppObjects[0]->GetMesh();
	pSphereMesh->RenderInstanced(pd3dDeviceContext, nSphereInstances, 0);

	int nCubeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dCubeInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = nSphereObjects; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				pCubeInstances[nCubeInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dCubeInstanceBuffer, 0);

	CMesh *pCubeMesh = m_ppObjects[m_nObjects - 1]->GetMesh();
	pCubeMesh->RenderInstanced(pd3dDeviceContext, nCubeInstances, 0);

}

CGameObject *CShader::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	MESHINTERSECTINFO d3dxIntersectInfo;
	/*쉐이더 객체에 포함되는 모든 객체들에 대하여 픽킹 광선을 생성하고 
	객체의 바운딩 박스와의 교차를 검사한다. 교차하는 객체들 중에 
	카메라와 가장 가까운 객체의 정보와 객체를 반환한다.*/
	for (int i = 0; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if (pd3dxIntersectInfo) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}


CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

//133
void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

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

	D3DXVECTOR3 d3dxvScale(8.0f, 2.0f, 8.0f);
	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257, 257, d3dxvScale);
	m_ppObjects[0]->SetTexture(pTerrainTexture);

	//133
	CMaterial *pTerrainMaterial = new CMaterial();
	pTerrainMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.8f, 1.0f, 0.2f, 1.0f);
	pTerrainMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.3f, 0.1f, 1.0f);
	pTerrainMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pTerrainMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_ppObjects[0]->SetMaterial(pTerrainMaterial);
}

CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return((CHeightMapTerrain *)m_ppObjects[0]);
}

void CPlayerShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CMaterial *pPlayerMaterial = new CMaterial();
	pPlayerMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.6f, 0.2f, 0.2f, 1.0f);
	pPlayerMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	pPlayerMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pPlayerMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.2f, 1.0f);

	CCubeMeshIlluminated *pCubeMesh = new CCubeMeshIlluminated(pd3dDevice, 4.0f, 12.0f, 4.0f);
	
	CTerrainPlayer *pTerrainPlayer = new CTerrainPlayer(1);
	pTerrainPlayer->SetMesh(pCubeMesh);
	pTerrainPlayer->CreateShaderVariables(pd3dDevice);
	pTerrainPlayer->ChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);
	pTerrainPlayer->SetMaterial(pPlayerMaterial);

	m_ppObjects[0] = pTerrainPlayer;

	////1
	////Player의 재질설정
	//CMaterial * pPlayerMaterial = new CMaterial();
	//pPlayerMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//pPlayerMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//pPlayerMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//pPlayerMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	////1
	//CAnimationController * pAnimationController = new CAnimationController(0.0088f);
	//CTerrainPlayer * pTerrainPlayer = new CTerrainPlayer(1);
	//pTerrainPlayer->AddRef();
	//pTerrainPlayer->SetMaterial(pPlayerMaterial);
	//pTerrainPlayer->SetMesh((*vpCharacters.begin())->m_pModelMesh);
	//pTerrainPlayer->SetTexture((*vpCharacters.begin())->m_pModelTexture);
	//pTerrainPlayer->ChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);
	////pTerrainPlayer->SetPlayerCharacterStatus();

	////1
	//CCharacterShader *pPlayerShader = new CCharacterShader(1);
	//pTerrainPlayer
}

CIlluminatedShader::CIlluminatedShader()
{
}

CIlluminatedShader::~CIlluminatedShader()
{
}

void CIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

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

void CIlluminatedShader::ReleaseShaderVariables()
{
	if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release();
}

void CIlluminatedShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}

CDiffusedShader::CDiffusedShader()
{
}

CDiffusedShader::~CDiffusedShader()
{
}

void CDiffusedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_5_0", &m_pd3dPixelShader);
}

CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

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

//131
CDetailTexturedShader::CDetailTexturedShader()
{
}

//131
CDetailTexturedShader::~CDetailTexturedShader()
{
}

//131
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

//132
CSkyBoxShader::CSkyBoxShader()
{
}

//132
CSkyBoxShader::~CSkyBoxShader()
{
}

//132
void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CSkyBox *pSkyBox = new CSkyBox(pd3dDevice);
	m_ppObjects[0] = pSkyBox;
}

//132
void CSkyBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::OnPrepareRender(pd3dDeviceContext);

	m_ppObjects[0]->Render(pd3dDeviceContext, pCamera);
}

//133
CTexturedIlluminatedShader::CTexturedIlluminatedShader()
{
}

//133
CTexturedIlluminatedShader::~CTexturedIlluminatedShader()
{
}

//133
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

//133
CDetailTexturedIlluminatedShader::CDetailTexturedIlluminatedShader()
{
}

//133
CDetailTexturedIlluminatedShader::~CDetailTexturedIlluminatedShader()
{
}

//133
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