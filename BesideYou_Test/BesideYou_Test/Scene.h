#pragma once

//#include "Shader.h"

class CScene
{
private:
	//씬은 쉐이더들의 리스트(배열)이다.
	CShader **m_ppShaders;
	int m_nShaders;

	//2.23
	CCamera *m_pCamera;
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);

public:
	D3DXVECTOR3 ReturnCubePosition ( int object )
	{
		return m_ppShaders[0]->ReturnCubePosition(object);
	}

//HeightMap관련
//2.23
public:
	CHeightMapTerrain * GetTerrain();

public:
	void SetCamera(CCamera *pCamera) {
		m_pCamera = pCamera;
	}
};