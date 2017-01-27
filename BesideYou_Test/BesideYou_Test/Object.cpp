#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

//0720
CGameObject::CGameObject(int nMeshes)
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	
	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0) m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;
	//m_pShader = NULL; �����Ѵ�.

	m_nReferences = 0;
}

//0720
CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}	
	
}

void CGameObject::AddRef()
{
	m_nReferences++;
}

void CGameObject::Release()
{
	if (m_nReferences > 0) m_nReferences--;
	if (m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh *pMesh, int nIndex)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) 
			m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::Animate(float fTimeElapsed)
{
}

//07100
void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

//07100
void CGameObject::SetPosition(D3DXVECTOR3 d3dxvPosition)
{
	m_d3dxmtxWorld._41 = d3dxvPosition.x;
	m_d3dxmtxWorld._42 = d3dxvPosition.y;
	m_d3dxmtxWorld._43 = d3dxvPosition.z;
}

//07100
D3DXVECTOR3 CGameObject::GetPosition()
{
	return(D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43));
}

//0720
void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	/*	if (m_pShader)
	{
	m_pShader->UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);
	m_pShader->Render(pd3dDeviceContext);
	} �����Ѵ�. */
	OnPrepareRender();
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dDeviceContext);
		}
	}
}

//072
D3DXVECTOR3 CGameObject::GetLookAt()
{
	//���� ��ü�� ���� z-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);
	return(d3dxvLookAt);
}

//072
D3DXVECTOR3 CGameObject::GetUp()
{
	//���� ��ü�� ���� y-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvUp(m_d3dxmtxWorld._21, m_d3dxmtxWorld._22, m_d3dxmtxWorld._23);
	D3DXVec3Normalize(&d3dxvUp, &d3dxvUp);
	return(d3dxvUp);
}

//072
D3DXVECTOR3 CGameObject::GetRight()
{
	//���� ��ü�� ���� x-�� ���͸� ��ȯ�Ѵ�.
	D3DXVECTOR3 d3dxvRight(m_d3dxmtxWorld._11, m_d3dxmtxWorld._12, m_d3dxmtxWorld._13);
	D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
	return(d3dxvRight);
}

//072
void CGameObject::MoveStrafe(float fDistance)
{
	//���� ��ü�� ���� x-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvRight = GetRight();
	d3dxvPosition += fDistance * d3dxvRight;
	CGameObject::SetPosition(d3dxvPosition);
}

//072
void CGameObject::MoveUp(float fDistance)
{
	//���� ��ü�� ���� y-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvUp = GetUp();
	d3dxvPosition += fDistance * d3dxvUp;
	CGameObject::SetPosition(d3dxvPosition);
}

//072
void CGameObject::MoveForward(float fDistance)
{
	//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt = GetLookAt();
	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

//072
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	//���� ��ü�� �־��� ������ ȸ���Ѵ�.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(fYaw), (float)D3DXToRadian(fPitch), (float)D3DXToRadian(fRoll));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

//072
void CGameObject::Rotate(D3DXVECTOR3 *pd3dxvAxis, float fAngle)
{
	//���� ��ü�� �־��� ȸ������ �߽����� ȸ���Ѵ�.
	D3DXMATRIX mtxRotate;
	D3DXMatrixRotationAxis(&mtxRotate, pd3dxvAxis, (float)D3DXToRadian(fAngle));
	m_d3dxmtxWorld = mtxRotate * m_d3dxmtxWorld;
}

//072
CRotatingObject::CRotatingObject(int nMeshes) : CGameObject(nMeshes)
{
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{
}

//072
void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_d3dxvRotationAxis, m_fRotationSpeed * fTimeElapsed);
}


void CRotatingObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

CHeightMapTerrain::CHeightMapTerrain(ID3D11Device *pd3dDevice, int nWidth, int nLength,
	int nBlockWidth, int nBlockLength, LPCTSTR pFileName, D3DXVECTOR3 d3dxvScale, D3DXCOLOR d3dxColor)
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = nWidth;
	m_nLength = nLength;

	//���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. 
	//nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ���̴�. 
	//cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.
	int cxQuadPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	//d3dxvScale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_d3dxvScale = d3dxvScale;

	//������ ����� ���̸��� �����Ѵ�.
	//m_pHeightMap = new CHeightMap(pFileName, nWidth, nLength, d3dxvScale);

	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ°��� ��Ÿ����.
	int cxBlocks = (m_nWidth - 1) / cxQuadPerBlock;
	int czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	////���� ��ü�� ǥ���ϱ����� ���� �޽��� �����̴�.
	m_nMeshes = cxBlocks * czBlocks;

	////���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�.
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;

	CHeightMapGridMesh * pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ�̴�.
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�.
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, xStart, zStart, nBlockWidth, nBlockLength, d3dxvScale, d3dxColor, 0);
			SetMesh(pHeightMapGridMesh, x + (z*cxBlocks));
		}
	}


	
}

CHeightMapTerrain::~CHeightMapTerrain()
{
}