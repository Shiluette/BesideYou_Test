#pragma once
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Camera.h"

#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x10
#define DIR_DOWN	0x20

//������ ������ ��Ÿ���� ����ü�̴�.
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
	D3DXCOLOR m_d3dxcEmissive;
};

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

	//1
	CTexture						*m_pTexture;
	void SetTexture(CTexture *pTexture);

};

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

class CShader;

class CGameObject
{
protected:
	CShader * m_pShader;

public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

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

	//��ü�� ī�޶��� ����ü ���ο� �ִ� ���� �Ǵ��Ѵ�. 
	bool IsVisible(CCamera *pCamera = nullptr);
	
private:
	int m_nReferences;
	
	//1
	CAnimationController*			m_pAnimationController;

	bool m_bActive;
public:
	void AddRef();
	void Release();
	D3DXMATRIX m_d3dxmtxWorld;
	
	
	virtual void Animate(float fTimeElapsed);
	//�޽� ������ ����ü �ø��� �ϱ� ���Ͽ� ī�޶� �ʿ��ϴ�.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

public:
	//��ü�� ��ġ�� �����Ѵ�.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	D3DXVECTOR3 GetPosition();
	
public:
	void SetActive(bool bActive = false) { m_bActive = bActive; }

	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, 
		D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition,
		D3DXVECTOR3 *pd3dxvPickRayDirection);
	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition,
		D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

	//1
	void SetAnimationController(CAnimationController* pAnimationController) { m_pAnimationController = pAnimationController; }

public:
	//��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh **m_ppMeshes;
	int m_nMeshes;

	//��ü�� ������ �޽� ��ü�� ���� �ٿ�� �ڽ��̴�.
	AABB m_bcMeshBoundingCube;

	void SetMesh(CMesh *pMesh, int nIndex = 0);
	CMesh *GetMesh(int nIndex = 0) { return(m_ppMeshes[nIndex]); };

	

public:
	//���� ��ü�� �ϳ��� ������ ���� �� �ִ�.
	CMaterial *m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);

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

	//���� �ӵ��� ȸ���� ���͸� �����ϴ� �Լ��̴�.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
};

class CRevolvingObject : public CGameObject
{
public:
	CRevolvingObject(int nMeshes = 1);
	virtual ~CRevolvingObject();

	virtual void Animate(float fTimeElapsed);

private:
	// ���� ȸ����� ȸ�� �ӷ��� ��Ÿ����.
	D3DXVECTOR3 m_d3dxvRevolutionAxis;
	float m_fRevolutionSpeed;

public:
	// ���� �ӷ��� �����Ѵ�.
	void SetRevolutionSpeed(float fRevolutionSpeed) { m_fRevolutionSpeed = fRevolutionSpeed; }
	// ������ ���� ȸ������ �����Ѵ�.
	void SetRevolutionAxis(D3DXVECTOR3 d3dxvRevolutionAxis) { m_d3dxvRevolutionAxis = d3dxvRevolutionAxis; }
};

//CHeightMap Ŭ������ ������ ���� �����Ѵ�. 
//�� Ŭ������ ���� �� �̹����� ��Ÿ����.
class CHeightMap
{
private:
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�.
	BYTE *m_pHeightMapImage;
	//���� �� �̹����� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;
	//���� ���� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�.
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

//CHeightMapTerrain Ŭ������ ������ ���� �����Ѵ�. 
//�� Ŭ������ ���� ���� ����� �ǿ� ������ ��Ÿ����.
class CHeightMapTerrain : public CGameObject
{
public:
	//CHeightMapTerrain Ŭ������ �����ڸ� ������ ���� �����Ѵ�.
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

private:
	//������ ���� ������ ����� �̹����̴�.
	CHeightMap *m_pHeightMap;

	//������ ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;

	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	D3DXVECTOR3 m_d3dxvScale;

public:
	//������ ���� ���̸� ��ȯ�Ѵ�. ���� ���� ���̿� �������� ���� ���̴�.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	//������ ���� ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }

	float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }
};

//132
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D11Device *pd3dDevice);
	virtual ~CSkyBox();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

//1
struct ModelContainer
{
	string		m_sModelName;
	CMesh*		m_pModelMesh;
	CTexture*	m_pModelTexture;

	void AddRef()
	{
		if(m_pModelMesh)
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