#pragma once

//#include "Shader.h"

//2.25
#define MAX_LIGHTS		4 
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//2.25
//1���� ������ ǥ���ϴ� ����ü�̴�. 
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

//2.25
//��� ���۴� ũ�Ⱑ �ݵ�� 16 ����Ʈ�� ����� �Ǿ�� �Ѵ�. 
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	D3DXCOLOR m_d3dxcGlobalAmbient;
	D3DXVECTOR4 m_d3dxvCameraPosition;
};

class CScene
{
private:
	//���� ���̴����� ����Ʈ(�迭)�̴�.
	CShader **m_ppShaders;
	int m_nShaders;

	//2.23
	CCamera *m_pCamera;

	//2.25
	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;
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

//HeightMap����
//2.23
public:
	CHeightMapTerrain * GetTerrain();

public:
	void SetCamera(CCamera *pCamera) {
		m_pCamera = pCamera;
	}

//Light����
//2.25
public:
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();

};