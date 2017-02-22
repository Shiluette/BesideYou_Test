#include "stdafx.h"
//#include "Scene.h"

//0720
CScene::CScene()
{
	m_ppShaders = NULL;
	m_nShaders = 0;
}

CScene::~CScene()
{
}

//0720
void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	/*m_nShaders = 1;
	m_pShaders = new CSceneShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice);
	m_pShaders[0].BuildObjects(pd3dDevice);*/

	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];

	m_ppShaders[0] = new CSceneShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);

	m_ppShaders[1] = new CTerrainShader();
	m_ppShaders[1]->CreateShader(pd3dDevice);
	m_ppShaders[1]->BuildObjects(pd3dDevice);


}

//0720
void CScene::ReleaseObjects()
{
	for (int j = 0; j < m_nShaders; j++) m_ppShaders[j]->ReleaseObjects();
	if (m_ppShaders) delete[] m_ppShaders;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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

//0720
void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(fTimeElapsed);
	}
}

//0720
void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
	}
}