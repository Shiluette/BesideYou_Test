#pragma once

#include "Camera.h"
#include "Mesh.h"

class CGameObject
{
private:
	int m_nReferences;

protected:
	D3DXMATRIX m_d3dxmtxWorld;
	CMesh **m_ppMeshes;
	int m_nMeshes;

public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

	void AddRef();
	void Release();

	virtual void SetMesh(CMesh *pMesh, int nindex = 0);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//07100
	//��ü�� ��ġ�� �����Ѵ�.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

//072
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
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

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

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
};