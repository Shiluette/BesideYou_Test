#pragma once

//#include "Shader.h"

class CScene
{
private:
	//씬은 쉐이더들의 리스트(배열)이다.
	CShader **m_ppShaders;
	int m_nShaders;
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

	D3DXVECTOR3 ReturnCubePosition ( int object )
	{
		return m_ppShaders[0]->ReturnCubePosition(object);
	}
};