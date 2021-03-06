#pragma once

//#include "Camera.h"
//#include "Mesh.h"

//2.25
//재질의 색상을 나타내는 구조체이다.
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
//게임 객체는 하나 이상의 텍스쳐를 가질 수 있다. CTexture는 텍스쳐를 관리하기 위한 클래스이다.
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
	//텍스쳐 리소스의 개수이다.
	int m_nTextures;
	ID3D11ShaderResourceView **m_ppd3dsrvTextures;
	//텍스쳐 리소스를 연결할 시작 슬롯이다.
	int m_nTextureStartSlot;
	//샘플러 상태 객체의 개수이다.
	int m_nSamplers;
	ID3D11SamplerState **m_ppd3dSamplerStates;
	//샘플러 상태 객체를 연결할 시작 슬롯이다.
	int m_nSamplerStartSlot;

public:
	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);
	//텍스쳐 리소스와 샘플러 상태 객체에 대한 쉐이더 변수를 변경한다.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	//텍스쳐 리소스에 대한 쉐이더 변수를 변경한다.
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	//샘플러 상태 객체에 대한 쉐이더 변수를 변경한다.
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
	//객체가 가지는 메쉬 전체에 대한 바운딩 박스이다.
	AABB m_bcMeshBoundingCube;

public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxRotate;
	D3DXMATRIX m_d3dxmtxScale;
	D3DXMATRIX m_d3dxmtxTranlate;

	virtual void SetMesh(CMesh *pMesh, int nindex = 0);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	//객체의 위치를 설정한다.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

	//3.3
	CMesh *GetMesh(int nIndex = 0) { 
		return(m_ppMeshes[nIndex]); 
	}

public:
	//로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//로컬 x-축, y-축, z-축 방향으로 회전한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle);

	//객체의 위치, 로컬 x-축, y-축, z-축 방향 벡터를 반환한다.
	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();

	//객체를 렌더링하기 전에 호출되는 함수이다.
	virtual void OnPrepareRender() { }

//2.23-1
//절두체컬링
//객체가 카메라의 절두체 내부에 있는 가를 판단한다. 
public:
	bool IsVisible(CCamera *pCamera = NULL);
private:
	bool m_bActive;

//2.25
public:
	//게임 객체는 하나의 재질을 가질 수 있다.
	CMaterial *m_pMaterial;
	void SetMaterial(CMaterial *pMaterial);

//2.26
public:
	//게임 객체는 텍스쳐 가질 수 있다.
	CTexture *m_pTexture;
	void SetTexture(CTexture *pTexture);
};

class CRotatingObject : public CGameObject
{
protected:
	//자전 속도와 회전축 벡터를 나타내는 멤버 변수를 선언한다.
	float m_fRotationSpeed;
	D3DXVECTOR3 m_d3dxvRotationAxis;
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	//자전 속도와 회전축 벡터를 설정하는 함수이다.
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

	//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.
	float GetHeight(float x, float z, bool bReverseQuad = false);
	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	D3DXVECTOR3 GetHeightMapNormal(int x, int z);
	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	BYTE *GetHeightMapImage() { return(m_pHeightMapImage); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapTerrain : public CGameObject
{
private:
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMap * m_pHeightMap;

	int m_nWidth;
	int m_nLength;

	D3DXVECTOR3 m_d3dxvScale;

public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice = NULL, int nWidth = 0, int nLength = 0,
		int nBlockWidth = 0, int nBlockLength = 0, LPCTSTR pFileName = NULL, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXCOLOR d3dxColor = (0.5f, 0.5f, 0.5f));
	virtual ~CHeightMapTerrain();

	//지형의 실제 높이를 반환한다. 높이 맵의 높이에 스케일을 곱한 값이다.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); };

	//2.23
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x);}
	float GetLength() { return(m_nLength * m_d3dxvScale.z);}
	float GetPeakHeight() {	return(m_bcMeshBoundingCube.m_d3dxvMaximum.y);}

	//3.3
	virtual void Animate(float fTimeElapsed);
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