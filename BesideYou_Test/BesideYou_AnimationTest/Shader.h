#pragma once

#include "Object.h"
#include "Camera.h"
#include "Player.h"

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX m_d3dxmtxWorld;
};
/*�ν��Ͻ� �����͸� ��Ÿ���� ����ü VS_VB_INSTANCE�� ������ ���� ����*/
struct VS_VB_INSTANCE
{
	D3DXMATRIX m_d3dxTransform;
	D3DXCOLOR m_d3dxColor;
};

class CShader
{
private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	
	CShader();
	virtual ~CShader();

	void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	//�����ϵ� ����Ʈ ���Ͽ��� ���� ���̴��� �ȼ� ���̴��� �����ϴ� �Լ��̴�.
	void CreateVertexShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements);
	void CreatePixelShaderFromCompiledFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName);

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

protected:
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11InputLayout *m_pd3dVertexLayout;

	ID3D11PixelShader *m_pd3dPixelShader;

	//���̴� ��ü�� ���� ��ü���� ����Ʈ�� ������.
	CGameObject **m_ppObjects;
	int m_nObjects;

	//���� ��ȯ ����� ���� ��� ���۴� �ϳ��� �־ �ǹǷ� ���� ����� �����Ѵ�.
	static ID3D11Buffer *m_pd3dcbWorldMatrix;

public:
	virtual CGameObject *PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, 
		D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);
};

//k
class CObjectsShader : public CShader
{
public:
	CObjectsShader(int nObjects = 1);
	virtual ~CObjectsShader();

protected:
	vector<CGameObject*> m_vpObjects;
	
	//������Ʈ���̴��� ������ ������ �ִ�.
	CMaterial * m_pMaterial;

};

//k
class CCharacterShader : public CObjectsShader
{
public:
	CCharacterShader(int nObjects = 1);
	~CCharacterShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//k
	virtual void BuildObjects(ID3D11Device *pd3dDevice, vector<ModelContainer*> vtCharacterData);
};

class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
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

class CTexturedShader : public CShader
{
public:
	CTexturedShader();
	virtual ~CTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};



//�÷��̾ �������ϱ� ���� ���̴� Ŭ�����̴�.
class CPlayerShader : public CIlluminatedShader
{
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	//void BuildObjects(ID3D11Device * pd3dDevice, vector<ModelContainer*>&);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera = NULL);

	CPlayer *GetPlayer(int nIndex = 0) { return((CPlayer *)m_ppObjects[nIndex]); }
};

//CInstancingShader Ŭ������ ���̽� Ŭ������ CIlluminatedShader�� �����Ѵ�.
class CInstancingShader : public CTexturedShader
{

private:
	CMaterial *m_pMaterial;
	CTexture *m_pTexture;
public:
	CInstancingShader();
	virtual ~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	virtual void BuildObjects(ID3D11Device *pd3dDevice, CHeightMapTerrain *pHeightMapTerrain,
		CMaterial *pMaterial, CTexture *pTexture, int k);
private:
	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	ID3D11Buffer *m_pd3dCubeInstanceBuffer;
	ID3D11Buffer *m_pd3dSphereInstanceBuffer;

public:
	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, 
		int nObjects, UINT nBufferStride, void *pBufferData);

};

//131
class CDetailTexturedShader : public CTexturedShader
{
public:
	CDetailTexturedShader();
	virtual ~CDetailTexturedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//132
class CSkyBoxShader : public CTexturedShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

//133
class CTexturedIlluminatedShader : public CIlluminatedShader
{
public:
	CTexturedIlluminatedShader();
	virtual ~CTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);

	//2.21
	virtual void BuildObjects(ID3D11Device *pd3dDevice, vector<ModelContainer*> vtCharacterData);
};

//133
class CDetailTexturedIlluminatedShader : public CTexturedIlluminatedShader
{
public:
	CDetailTexturedIlluminatedShader();
	virtual ~CDetailTexturedIlluminatedShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};

//133
//CTerrainShader Ŭ������ ���̽� Ŭ������ CDetailTexturedIlluminatedShader Ŭ������ �����Ѵ�.
class CTerrainShader : public CDetailTexturedIlluminatedShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	CHeightMapTerrain *GetTerrain();
};
