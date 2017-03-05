#include "stdafx.h"

//#include "Scene.h"

//3.5
static bool bCharaterRun = false;
static bool bCharaterPunch = false;

CScene::CScene()
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	//2.25
	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}

CScene::~CScene()
{
}

//2.25
void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 300.0f;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvPosition = D3DXVECTOR3(300.0f, 300.0f, 300.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = 1.0f;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcDiffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[1].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	m_pLights->m_pLights[2].m_bEnable = 1.0f;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[2].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[2].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = 0.0f;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_d3dxcAmbient = D3DXCOLOR(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcDiffuse = D3DXCOLOR(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
	m_pLights->m_pLights[3].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	m_pLights->m_pLights[3].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 20.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(D3DXToRadian(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(D3DXToRadian(15.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}

//2.25
void CScene::ReleaseShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}

//2.25
void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	//3.3
	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];

	m_ppShaders[0] = new CSceneShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);

	m_ppShaders[1] = new CTerrainShader();
	m_ppShaders[1]->CreateShader(pd3dDevice);
	m_ppShaders[1]->BuildObjects(pd3dDevice);

	{
		CMaterial *pNormalMaterial = new CMaterial();
		pNormalMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
		pNormalMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pNormalMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
		pNormalMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

		//3.3
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

		//3.3
		ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
		CTexture *pDrayerTexture = new CTexture(1, 1, 0, 0);
		D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("../Data/drayer_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
		pDrayerTexture->SetTexture(0, pd3dsrvTexture);
		pDrayerTexture->SetSampler(0, pd3dSamplerState);
		pd3dsrvTexture->Release();
		pd3dsrvTexture = NULL;

		//3.3
		CMesh *pTestMesh = new CFBXMesh(pd3dDevice, "../Data/drayer_animation.data", 0.1);

		m_ppShaders[2] = new CCharacterShader(1);
		m_ppShaders[2]->CreateShader(pd3dDevice);
		m_ppShaders[2]->BuildObjects(pd3dDevice);

		CGameObject * pTestObject = new CGameObject(1);
		pTestObject->SetMesh(pTestMesh);
		pTestObject->SetMaterial(pNormalMaterial);
		pTestObject->SetTexture(pDrayerTexture);
		pTestObject->Rotate(-90.0f, 0.0f, 0.0f);
		pTestObject->SetPosition(1028.0f, 0.0f, 1028.0f);
		
		//AddObject는 BuildObjects에서 Object를 초기화하지않기때문에 따로 만들어준 함수
		m_ppShaders[2]->AddObject(pTestObject);
	}

	//2.25
	//조명설정
	CreateShaderVariables(pd3dDevice);
}

void CScene::ReleaseObjects()
{
	//2.25
	ReleaseShaderVariables();

	for (int j = 0; j < m_nShaders; j++)
	{
		if (m_ppShaders[j]) m_ppShaders[j]->ReleaseObjects();
		if (m_ppShaders[j]) delete m_ppShaders[j];
	}
	if (m_ppShaders) delete[] m_ppShaders;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	


	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//3.5
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'Z':
			if (!bCharaterRun && !bCharaterPunch)
			{
				m_ppShaders[2]->GetFBXMesh->SetAnimation(1);
				bCharaterRun = true;
				bCharaterPunch = false;
			}
			break;
		case 'X':
			if (!bCharaterPunch)
			{
				m_ppShaders[2]->GetFBXMesh->SetAnimation(2);
				bCharaterRun = false;
				bCharaterPunch = true;
			}
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case 'Z':
			if (bCharaterRun)
			{
				m_ppShaders[2]->GetFBXMesh->SetAnimation(0);
				bCharaterRun = false;
			}
			break;
		case 'X':
			if (bCharaterPunch)
			{
				m_ppShaders[2]->GetFBXMesh->SetAnimation(0);
				bCharaterPunch = false;
			}
		default:
			break;
		}
		break;
	default:
		break;
	}

	return(false);
}

bool CScene::ProcessInput()
{
	if (GetKeyState('D') & 0xFF00)
		m_ppShaders[0]->MoveObject(0,"RIGHT");
	if (GetKeyState('A') & 0xFF00)
		m_ppShaders[0]->MoveObject(0, "LEFT");
	if (GetKeyState('W') & 0xFF00)
		m_ppShaders[0]->MoveObject(0, "BACK");
	if (GetKeyState('S') & 0xFF00)
		m_ppShaders[0]->MoveObject(0, "FRONT");

	if (GetKeyState('L') & 0xFF00)
		m_ppShaders[0]->MoveObject(1, "RIGHT");
	if (GetKeyState('J') & 0xFF00)
		m_ppShaders[0]->MoveObject(1, "LEFT");
	if (GetKeyState('I') & 0xFF00)
		m_ppShaders[0]->MoveObject(1, "BACK");
	if (GetKeyState('K') & 0xFF00)
		m_ppShaders[0]->MoveObject(1, "FRONT");

	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	//2.25
	if (m_pLights && m_pd3dcbLights)
	{
		//현재 카메라의 위치 벡터를 조명을 나타내는 상수 버퍼에 설정한다.
		D3DXVECTOR3 d3dxvCameraPosition = m_pCamera->GetPosition();
		m_pLights->m_d3dxvCameraPosition = D3DXVECTOR4(d3dxvCameraPosition, 1.0f);

		//점 조명이 지형의 중앙을 중심으로 회전하도록 설정한다.
		CHeightMapTerrain *pTerrain = GetTerrain();
		static D3DXVECTOR3 d3dxvRotated = D3DXVECTOR3(pTerrain->GetWidth()*0.3f, 0.0f, 0.0f);
		D3DXMATRIX d3dxmtxRotate;
		D3DXMatrixRotationYawPitchRoll(&d3dxmtxRotate, (float)D3DXToRadian(30.0f*fTimeElapsed), 0.0f, 0.0f);
		D3DXVec3TransformCoord(&d3dxvRotated, &d3dxvRotated, &d3dxmtxRotate);
		D3DXVECTOR3 d3dxvTerrainCenter = D3DXVECTOR3(pTerrain->GetWidth()*0.5f, pTerrain->GetPeakHeight() + 10.0f, pTerrain->GetLength()*0.5f);

		m_pLights->m_pLights[0].m_d3dxvPosition = d3dxvTerrainCenter + d3dxvRotated;
		m_pLights->m_pLights[0].m_fRange = pTerrain->GetPeakHeight();

		/*두 번째 조명은 플레이어가 가지고 있는 손전등(스팟 조명)이다. 그러므로 플레이어의 위치와 방향이 바뀌면 현재 플레이어의 위치와 z-축 방향 벡터를 스팟 조명의 위치와 방향으로 설정한다.*/
		CPlayer *pPlayer = m_pCamera->GetPlayer();
		//캐릭터가 가지고 있는 스팟조명
		m_pLights->m_pLights[1].m_d3dxvPosition = pPlayer->GetPosition();
		m_pLights->m_pLights[1].m_d3dxvDirection = pPlayer->GetLookVector();

		//위에서 캐릭터를 향하는 스팟조명
		m_pLights->m_pLights[3].m_d3dxvPosition = pPlayer->GetPosition() + D3DXVECTOR3(0.0f, 40.0f, 0.0f);
	}

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}

	m_ppShaders[2]->GetFBXMesh->FBXFrameAdvance(fTimeElapsed);
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	//2.25
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);

	for (int i = 0; i < m_nShaders; i++)
	{
		//3.3
		if ( i == 2 )
			m_ppShaders[i]->GetFBXMesh->UpdateBoneTransform(pd3dDeviceContext, m_ppShaders[i]->GetFBXMesh->GetFBXAnimationNum(), m_ppShaders[i]->GetFBXMesh->GetFBXNowFrameNum());
		if ( i != 1 )
			m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
	}
}

//2.23
CHeightMapTerrain *CScene::GetTerrain()
{
	CTerrainShader *pTerrainShader = (CTerrainShader*)m_ppShaders[1];
	return (pTerrainShader->GetTerrain());
}


