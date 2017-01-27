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
	//객체의 위치를 설정한다.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);
	D3DXVECTOR3 GetPosition();

//072
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
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//자전 속도와 회전축 벡터를 설정하는 함수이다.
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(D3DXVECTOR3 d3dxvRotationAxis) { m_d3dxvRotationAxis = d3dxvRotationAxis; }
};

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice = NULL, int nWidth = 0, int nLength = 0,
		int nBlockWidth = 0, int nBlockLength = 0, LPCTSTR pFileName = NULL, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXCOLOR d3dxColor = (0.5f, 0.5f, 0.5f));
	virtual ~CHeightMapTerrain();

private:
	//지형의 높이 맵으로 사용할 이미지이다.
	//CHeightMap * m_pHeightMap;

	int m_nWidth;
	int m_nLength;

	D3DXVECTOR3 m_d3dxvScale;
};