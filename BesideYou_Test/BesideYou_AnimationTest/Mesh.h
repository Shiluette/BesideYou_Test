#pragma once

/*������ ������ ��������(Random) �����ϱ� ���� ����Ѵ�. 
�� ������ ������ ����(Random Number)�� �����Ͽ� �����Ѵ�.*/
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

//MESHINTERSECTINFO ����ü�� ������ ���� �����Ѵ�.
struct MESHINTERSECTINFO {
	DWORD m_dwFaceIndex;
	float m_fU;
	float m_fV;
	float m_fDistance;
};

class AABB
{
public:
	//�ٿ�� �ڽ��� �ּ����� �ִ����� ��Ÿ���� �����̴�.
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;

public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//�� ���� �ٿ�� �ڽ��� ���Ѵ�.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
	//�ٿ�� �ڽ��� 8���� �������� ��ķ� ��ȯ�ϰ� �ּ����� �ִ����� �ٽ� ����Ѵ�.
	void Update(D3DXMATRIX *pd3dxmtxTransform);
};

class CVertex
{
	//������ ��ġ ����(3���� ����)�� �����ϱ� 
	//���� ��� ������ �����Ѵ�. 
	D3DXVECTOR3 m_d3dxvPosition;
public:
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }
};

class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

private:
	int m_nReferences;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//Mesh.cpp ���Ͽ��� CMesh::AddRef(), CMesh::Release() �Լ��� ���Ǹ� �����Ѵ�. 

	//2.21
	void CalculateBoundingCube();

protected:
	AABB m_bcBoundingCube;

protected:
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//������ ��ġ ���Ϳ� ������ �����ϱ� ���� ���ۿ� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dPositionBuffer;

	//���۵��� �Է������⿡ �����ϱ� ���� ���� ���� ��ȣ�̴�. 
	UINT m_nSlot;
	/*�ν��Ͻ��� ���� ���� ���۴� �޽��� ���� �����Ϳ� �ν��Ͻ� ������(��ü�� ��ġ�� ����)�� ���´�. �׷��Ƿ� �ν��Ͻ��� ���� ���� ���۴� �ϳ��� �ƴ϶� ���۵��� �迭�̴�. ������ ��ҵ��� ��Ÿ���� ���۵��� �Է������⿡ �����ϱ� ���� �����̴�.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//������ �����ϱ� ���� �ʿ��� ������ �����̴�. 
	int m_nBuffers;

	//������ �����̴�. 
	int m_nVertices;
	UINT m_nStartVertex;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexStrides;
	//������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�. 
	UINT *m_pnVertexOffsets;

	//�ε��� ����(�ε����� �迭)�� ���� �������̽� �������̴�. 
	ID3D11Buffer *m_pd3dIndexBuffer;
	//�ε��� ���۰� �����ϴ� �ε����� �����̴�. 
	UINT m_nIndices;
	//�ε��� ���ۿ��� �޽��� ǥ���ϱ� ���� ���Ǵ� ���� �ε����̴�. 
	UINT m_nStartIndex;
	//�� �ε����� ������ �ε����̴�. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//�� �ε����� ����(DXGI_FORMAT_R32_UINT �Ǵ� DXGI_FORMAT_R16_UINT)�̴�. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	ID3D11RasterizerState *m_pd3dRasterizerState;

public:
	AABB GetBoundingCube() { return(m_bcBoundingCube); }

	//�޽��� ���� ���۵��� �迭�� �����Ѵ�. 
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) { }
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	//�ν��Ͻ��� ����Ͽ� �������Ѵ�. 
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);

	//1
	ID3D11Buffer *CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags);
public:
	/*�� ������ ��ġ ���͸� ��ŷ�� ���Ͽ� �����Ѵ�
	(���� ���۸� DYNAMIC���� �����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*�޽��� �ε����� �����Ѵ�(�ε��� ���۸� DYNAMIC���� 
	�����ϰ� Map()�� ���� �ʾƵ� �ǵ���).*/
	UINT *m_pnIndices;

	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, 
		D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);

};

class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	//������ ������ ��Ÿ���� ��� ����(D3DXCOLOR ����ü)�� �����Ѵ�. 
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//�����ڿ� �Ҹ��ڸ� �����Ѵ�.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

//CMeshDiffused Ŭ������ CMesh Ŭ�������� �Ļ��Ͽ� ������ ���� �����Ѵ�.
class CMeshDiffused : public CMesh
{
public:
	CMeshDiffused(ID3D11Device *pd3dDevice);
	virtual ~CMeshDiffused();

protected:
	ID3D11Buffer *m_pd3dColorBuffer;
};

/*CAirplaneMesh�� CAirplaneMeshDiffused�� �����ϰ� 
���̽� Ŭ������ CMeshDiffused�� �����Ѵ�.*/
class CAirplaneMeshDiffused : public CMeshDiffused
{
public:
	CAirplaneMeshDiffused(ID3D11Device *pd3dDevice, float fWidth = 20.0f, 
		float fHeight = 20.0f, float fDepth = 4.0f, 
		D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};

/*CCubeMesh�� CCubeMeshDiffused�� �����ϰ� 
���̽� Ŭ������ CMeshDiffused�� �����Ѵ�.*/
class CCubeMeshDiffused : public CMeshDiffused
{
public:
	CCubeMeshDiffused(ID3D11Device *pd3dDevice, float fWidth = 2.0f, 
		float fHeight = 2.0f, float fDepth = 2.0f, 
		D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CCubeMeshDiffused();
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	void CreateRasterizerState(ID3D11Device *pd3dDevice);
};

/*CSphereMesh�� CSphereMeshDiffused�� �����ϰ� ���̽� Ŭ������
CMeshDiffused�� �����Ѵ�.*/
class CSphereMeshDiffused : public CMeshDiffused
{
public:
	CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CSphereMeshDiffused();
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};

//CMeshIlluminated Ŭ������ ������ ���� �����Ѵ�.
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshIlluminated();

protected:
	//������ ������ ����ϱ� ���Ͽ� �������Ͱ� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dNormalBuffer;

public:
	//������ ���Ե� �ﰢ���� �������͸� ����ϴ� �Լ��̴�.
	D3DXVECTOR3 CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2);
	void SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
	//������ ���������� ����� ����ϴ� �Լ��̴�.
	void SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip);
	void CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals);
};

class CMeshTextured : public CMesh
{
public:
	CMeshTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshTextured();

protected:
	//�ؽ��� ������ �ϱ� ���Ͽ� �ؽ��� ��ǥ�� �ʿ��ϴ�.
	ID3D11Buffer *m_pd3dTexCoordBuffer;
};

/*�ؽ��� ������ ����Ͽ� ������ �����ϱ� ���Ͽ� ������ �ؽ��� ��ǥ�� ���� ������ü �޽� Ŭ�����̴�.*/
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

/*�ؽ��� ������ ����Ͽ� ������ �����ϱ� ���Ͽ� ������ �ؽ��� ��ǥ�� ���� �� �޽� Ŭ�����̴�.*/
class CSphereMeshTextured : public CMeshTextured
{
public:
	CSphereMeshTextured(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTextured();
};

//131
class CMeshDetailTextured : public CMeshTextured
{
public:
	CMeshDetailTextured(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTextured();

protected:
	ID3D11Buffer *m_pd3dDetailTexCoordBuffer;
};

//������ ����Ͽ� ������ ������ �����ϱ� ���Ͽ� ������ 
//�������͸� ���� ������ü �޽� Ŭ�����̴�.
class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};

//������ ����Ͽ� ������ ������ �����ϱ� ���Ͽ�
//������ �������͸� ���� �� �޽� Ŭ�����̴�.
class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

//132
class CTexture;

class CSkyBoxMesh : public CMeshTextured
{
protected:
	ID3D11DepthStencilState *m_pd3dDepthStencilState;

	CTexture *m_pSkyboxTexture;

public:
	CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();

	void OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, int nIndex = 0);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//133
class CMeshTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshTexturedIlluminated();

protected:
	ID3D11Buffer *m_pd3dTexCoordBuffer;
};

//133
class CMeshDetailTexturedIlluminated : public CMeshIlluminated
{
public:
	CMeshDetailTexturedIlluminated(ID3D11Device *pd3dDevice);
	virtual ~CMeshDetailTexturedIlluminated();

protected:
	ID3D11Buffer *m_pd3dTexCoordBuffer;
	ID3D11Buffer *m_pd3dDetailTexCoordBuffer;
};

//133
class CCubeMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CCubeMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTexturedIlluminated();
};

//133
class CSphereMeshTexturedIlluminated : public CMeshTexturedIlluminated
{
public:
	CSphereMeshTexturedIlluminated(ID3D11Device *pd3dDevice, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshTexturedIlluminated();
};

//133
//CHeightMapGridMesh Ŭ������ ���̽� Ŭ������ CMeshDetailTexturedIlluminated�� �����Ѵ�.
class CHeightMapGridMesh : public CMeshDetailTexturedIlluminated
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�.
	int m_nWidth;
	int m_nLength;
	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�.
	���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ, z-��ǥ�� ������ ������ x-��ǥ,
	y-��ǥ, z-��ǥ�� ���� ���� ���´�. ��, ���� ������ x-�� ������ ������ 1��
	�ƴ϶� ������ ������ x-��ǥ�� �ȴ�. �̷��� �ϸ� ���� ���ڸ� ����ϴ���
	ū ���ڸ� ������ �� �ִ�.*/
	D3DXVECTOR3 m_d3dxvScale;

public:
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength,
		D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//������ ����(����)�� ���̸� �����Ѵ�.
	virtual float OnGetHeight(int x, int z, void *pContext);
	//������ ����(����)�� ������ �����Ѵ�.
	virtual D3DXCOLOR OnGetColor(int x, int z, void *pContext);
};

//2.21
class CAssetMesh : public CMeshTexturedIlluminated
{
	unsigned int m_uitype;//ĳ���Ϳ� ���ӵǴ� �޽�Ÿ���� ���, �ո� ���⸦ ������Ű�� ���� ������ offset�� �ʿ��ϴ�.
public:
	enum {
		Common = 0,
		CharacterWeapon = 1
	};
	CAssetMesh(ID3D11Device *pd3dDevice, const std::string& filename, unsigned int type = CAssetMesh::Common);
	virtual ~CAssetMesh();
};