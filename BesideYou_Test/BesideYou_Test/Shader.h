#pragma once

//#include "Object.h"
//#include "Camera.h"
//#include "Player.h"

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};

class CShader
{
public:
	CShader();
	virtual ~CShader();

	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);

	//게임 객체들을 생성하고 애니메이션 처리를 하고 렌더링하기 위한 함수이다.
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);
	
	void MoveObject(int object, char * direction)
	{
		if ( direction == "RIGHT" )
			m_ppObjects[object]->MoveStrafe(0.1);
		if (direction == "LEFT")
			m_ppObjects[object]->MoveStrafe(-0.1);
		if (direction == "BACK")
			m_ppObjects[object]->MoveForward(0.1);
		if (direction == "FRONT")
			m_ppObjects[object]->MoveForward(-0.1);
	}

	D3DXVECTOR3 ReturnCubePosition(int object)
	{
		return m_ppObjects[object]->GetPosition();
	}

	//3.3
	virtual void AddObject(CGameObject *pGameObject);

	//3.3
	CGameObject* GetGameObject(int nIndex) { 
		return m_ppObjects[nIndex]; 
	}

protected:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;
	ID3D11PixelShader *m_pd3dPixelShader;

	//쉐이더 객체가 게임 객체들의 리스트를 가진다.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//3.3 이거뭐지?
	int m_nIndexToAdd;

	//월드 변환 행렬을 위한 상수 버퍼는 하나만 있어도 되므로 정적 멤버로 선언한다.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;
};

//게임 객체들을 렌더링하기 위한 쉐이더 클래스이다.
class CSceneShader : public CShader
{
public:
	CSceneShader();
	virtual ~CSceneShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
};

//2.25
class CIlluminatedShader : public CShader
{
public:
	CIlluminatedShader();
	virtual ~CIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//재질을 설정하기 위한 상수 버퍼이다.
	static ID3D11Buffer	 *m_pd3dcbMaterial;

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	//재질을 쉐이더 변수에 설정(연결)하기 위한 함수이다.
	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial);
};

//2.26
class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//2.27-1
class CDetailTexturedShader : public CTexturedShader
{
public:
	CDetailTexturedShader();
	virtual ~CDetailTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//2.27-2
class CTexturedIlluminatedShader : public CIlluminatedShader
{
public:
	CTexturedIlluminatedShader();
	virtual ~CTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//2.27-2
class CDetailTexturedIlluminatedShader : public CTexturedIlluminatedShader
{
public:
	CDetailTexturedIlluminatedShader();
	virtual ~CDetailTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//2.25-1 CShader를 CIlluminatedShader로 바꿈
//2.27 CIlluminatedShader를 CTexturedShader로 변경한다.
//2.27-1 CTexturedShader를 CDetailTexturedShader로 변경한다.
//2.27-2 CDetailTexturedShader를 CDetailTexturedIlluminatedShader로 변경한다.
class CTerrainShader : public CDetailTexturedIlluminatedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual void BuildObjects(ID3D11Device * pd3dDevice);

	//2.23
	CHeightMapTerrain * GetTerrain();
};

//2.25
//플레이어를 렌더링하기 위한 쉐이더 클래스이다.
//3.2
//플레이어는 이제 텍스쳐와 조명의 영향을 둘다 받아야하기때문에 CTexturedIlluminattedShader를 상속받는다.
class CPlayerShader : public CTexturedIlluminatedShader
{
	//2.25
private:
	CMaterial *m_pMaterial;

	//2.26
private:
	CTexture *m_pTexture;

public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	
	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	//3.2
	void BuildObjects(ID3D11Device *pd3dDevice, vector<ModelContainer*> vtCharacterData);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	CPlayer *GetPlayer(int nIndex = 0) { return((CPlayer *)m_ppObjects[nIndex]); }
};