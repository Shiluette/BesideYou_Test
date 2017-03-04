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

	//���� ��ü���� �����ϰ� �ִϸ��̼� ó���� �ϰ� �������ϱ� ���� �Լ��̴�.
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

	//���̴� ��ü�� ���� ��ü���� ����Ʈ�� ������.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//3.3 �̰Ź���?
	int m_nIndexToAdd;

	//���� ��ȯ ����� ���� ��� ���۴� �ϳ��� �־ �ǹǷ� ���� ����� �����Ѵ�.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;
};

//���� ��ü���� �������ϱ� ���� ���̴� Ŭ�����̴�.
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

	//������ �����ϱ� ���� ��� �����̴�.
	static ID3D11Buffer	 *m_pd3dcbMaterial;

	static void CreateShaderVariables(ID3D11Device *pd3dDevice);
	static void ReleaseShaderVariables();
	//������ ���̴� ������ ����(����)�ϱ� ���� �Լ��̴�.
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

//2.25-1 CShader�� CIlluminatedShader�� �ٲ�
//2.27 CIlluminatedShader�� CTexturedShader�� �����Ѵ�.
//2.27-1 CTexturedShader�� CDetailTexturedShader�� �����Ѵ�.
//2.27-2 CDetailTexturedShader�� CDetailTexturedIlluminatedShader�� �����Ѵ�.
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
//�÷��̾ �������ϱ� ���� ���̴� Ŭ�����̴�.
//3.2
//�÷��̾�� ���� �ؽ��Ŀ� ������ ������ �Ѵ� �޾ƾ��ϱ⶧���� CTexturedIlluminattedShader�� ��ӹ޴´�.
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