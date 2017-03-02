#pragma once

//#include "Camera.h"
//#include "Mesh.h"

//2.25
//������ ������ ��Ÿ���� ����ü�̴�.
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

//2.25
class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	MATERIAL m_Material;
};

//2.26
//���� ��ü�� �ϳ� �̻��� �ؽ��ĸ� ���� �� �ִ�. CTexture�� �ؽ��ĸ� �����ϱ� ���� Ŭ�����̴�.
class CTexture
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private:
	//�ؽ��� ���ҽ��� �����̴�.
	int m_nTextures;
	ID3D11ShaderResourceView **m_ppd3dsrvTextures;
	//�ؽ��� ���ҽ��� ������ ���� �����̴�.
	int m_nTextureStartSlot;
	//���÷� ���� ��ü�� �����̴�.
	int m_nSamplers;
	ID3D11SamplerState **m_ppd3dSamplerStates;
	//���÷� ���� ��ü�� ������ ���� �����̴�.
	int m_nSamplerStartSlot;

public:
	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	//�ؽ��� ���ҽ��� ���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	//�ؽ��� ���ҽ��� ���� ���̴� ������ �����Ѵ�.
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	//���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
	void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
};

class CGameObject
{
private:
	int m_nReferences;

protected:
	D3DXMATRIX m_d3dxmtxWorld;
	
	CMesh **m_ppMeshes;
	int m_nMeshes;

	//2.23-1
	//��ü�� ������ �޽� ��ü�� ���� �ٿ�� �ڽ��̴�.
	AABB m_bcMeshBoundingCube;

public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

	void AddRef();
	void Release();

	virtual void SetMesh(CMesh *pMesh, int nindex = 0);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	//��ü�� ��ġ�� �����Ѵ�.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

public:
	//���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//���� x-��, y-��, z-�� �������� ȸ���Ѵ�.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	//��ü�� ��ġ, ���� x-��, y-��, z-�� ���� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	//��ü�� �������ϱ� ���� ȣ��Ǵ� �Լ��̴�.
	virtual void OnPrepareRender() { }

//2.23-1
//����ü�ø�
//��ü�� ī�޶��� ����ü ���ο� �ִ� ���� �Ǵ��Ѵ�. 
public:
	bool IsVisible(CCamera *pCamera = NULL);
private:
	bool m_bActive;

//2.25
public:
	//���� ��ü�� �ϳ��� ������ ���� �� �ִ�.
	CMaterial *m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);

//2.26
public:
	//���� ��ü�� �ؽ��� ���� �� �ִ�.
	CTexture *m_pTexture;
	void SetTexture(CTexture *pTexture);
};

class CRotatingObject : public CGameObject
{
protected:
	//���� �ӵ��� ȸ���� ���͸� ��Ÿ���� ��� ������ �����Ѵ�.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	//���� �ӵ��� ȸ���� ���͸� �����ϴ� �Լ��̴�.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
};

class CHeightMap
{
	BYTE * m_pHeightMapImage;

	int m_nWidth;
	int m_nLength;

	D3DXVECTOR3 m_d3dxvScale;
public:
	CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale);
	~CHeightMap(void);

	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�.
	float GetHeight(float x, float z, bool bReverseQuad = false);
	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapTerrain : public CGameObject
{
private:
	//������ ���� ������ ����� �̹����̴�.
	CHeightMap * m_pHeightMap;

	int m_nWidth;
	int m_nLength;

	D3DXVECTOR3 m_d3dxvScale;

public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice = NULL, int nWidth = 0, int nLength = 0,
		int nBlockWidth = 0, int nBlockLength = 0, LPCTSTR pFileName = NULL, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXCOLOR d3dxColor = (0.5f, 0.5f, 0.5f));
	virtual ~CHeightMapTerrain();

	//������ ���� ���̸� ��ȯ�Ѵ�. ���� ���� ���̿� �������� ���� ���̴�.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); };

	//2.23
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x);}
	float GetLength() { return(m_nLength * m_d3dxvScale.z);}
	float GetPeakHeight() {	return(m_bcMeshBoundingCube.m_d3dxvMaximum.y);}
};

//3.2
struct ModelContainer
{
	string		m_sModelName;
	CMesh*		m_pModelMesh;
	CTexture*	m_pModelTexture;

	void AddRef()
	{
		if (m_pModelMesh)
			m_pModelMesh->AddRef();
		if (m_pModelTexture)
			m_pModelTexture->AddRef();
	}
	void Release()
	{
		m_pModelMesh->Release();
		m_pModelTexture->Release();
	}
};