#pragma once

#include "Camera.h"
#include "Shader.h"

#define MAX_LIGHTS		4 
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//1개의 조명을 표현하는 구조체이다. 
struct LIGHT
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular;
	D3DXVECTOR3 m_d3dxvPosition;
	float m_fRange;
	D3DXVECTOR3 m_d3dxvDirection;
	float m_nType;
	D3DXVECTOR3 m_d3dxvAttenuation;
	float m_fFalloff;
	float m_fTheta; //cos(m_fTheta)
	float m_fPhi; //cos(m_fPhi)
	float m_bEnable;
	float padding;
};

//상수 버퍼는 크기가 반드시 16 바이트의 배수가 되어야 한다. 
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	D3DXCOLOR m_d3dxcGlobalAmbient;
	D3DXVECTOR4 m_d3dxvCameraPosition;
};

class CScene
{
private:
	CShader **m_ppShaders;
	//씬은 여러 개의 다른 쉐이더 객체들을 포함한다. 
	int m_nShaders;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;
	
	CCamera *m_pCamera;	
	CGameObject *m_pSelectedObject;

public:
	CScene();
	~CScene();
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);

	//k
	void BuildObjects(ID3D11Device *pd3dDevice, vector<ModelContainer*> vtCharacterData);

	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

public:
	CGameObject *PickObjectPointedByCursor(int xClient, int yClient);
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();
	
public:
	CHeightMapTerrain *GetTerrain();

};
