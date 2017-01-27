#pragma once

class CVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
public:
	CVertex() { m_d3dxvPosition = D3DXVECTOR3(0, 0, 0); }
	CVertex(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }
	~CVertex() { }
};

//052
class CDiffusedVertex
{
	D3DXVECTOR3 m_d3dxvPosition;
	D3DXCOLOR m_d3dxcDiffuse;
public:
	//생성자와 소멸자를 선언한다.
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = D3DXVECTOR3(x, y, z); m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex(D3DXVECTOR3 d3dxvPosition, D3DXCOLOR d3dxcDiffuse) { m_d3dxvPosition = d3dxvPosition; m_d3dxcDiffuse = d3dxcDiffuse; }
	CDiffusedVertex() { m_d3dxvPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_d3dxcDiffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f); }
	~CDiffusedVertex() { }
};

class CMesh
{	
	int m_nReferences;
public:
	/*각 정점의 위치 벡터를 픽킹을 위하여 저장한다(정점 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/
	D3DXVECTOR3 *m_pd3dxvPositions;
	/*메쉬의 인덱스를 저장한다(인덱스 버퍼를 DYNAMIC으로 생성하고 Map()을 하지 않아도 되도록).*/
	UINT *m_pnIndices;

	//CMesh 클래스 객체의 참조(Reference)와 관련된 멤버 변수와 함수를 선언한다.

	
protected:
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;

	//정점의 위치 벡터와 색상을 저장하기 위한 버퍼에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dPositionBuffer;
	ID3D11Buffer *m_pd3dColorBuffer;

	//버퍼들을 입력조립기에 연결하기 위한 시작 슬롯 번호이다. 
	UINT m_nSlot;
	/*인스턴싱을 위한 정점 버퍼는 메쉬의 정점 데이터와 인스턴싱 데이터(객체의 위치와 방향)를 갖는다. 그러므로 인스턴싱을 위한 정점 버퍼는 하나가 아니라 버퍼들의 배열이다. 정점의 요소들을 나타내는 버퍼들을 입력조립기에 전달하기 위한 버퍼이다.*/
	ID3D11Buffer **m_ppd3dVertexBuffers;
	//정점을 조립하기 위해 필요한 버퍼의 개수이다. 
	int m_nBuffers;

	//정점의 개수이다. 
	int m_nVertices;
	UINT m_nStartVertex;
	//정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열이다. 
	UINT *m_pnVertexStrides;
	//정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다. 
	UINT *m_pnVertexOffsets;

	//인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 
	ID3D11Buffer *m_pd3dIndexBuffer;
	//인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	UINT m_nIndices;
	//인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	UINT m_nStartIndex;
	//각 인덱스에 더해질 인덱스이다. 
	int m_nBaseVertex;
	UINT m_nIndexOffset;
	//각 인덱스의 형식(DXGI_FORMAT_R32_UINT 또는 DXGI_FORMAT_R16_UINT)이다. 
	DXGI_FORMAT	m_dxgiIndexFormat;

	ID3D11RasterizerState *m_pd3dRasterizerState;
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	void AddRef();
	void Release();

	//정점 데이터를 렌더링하는 멤버 함수를 선언한다.
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//05-1
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);

	//메쉬의 정점 버퍼들을 배열로 조립한다. 
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	////--------------------------------------------------------------------------------
	////07 (내 생각에는 아래내용은 private로 바뀌어야할것같다)
	///*인덱스 버퍼(인덱스의 배열)에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점 버퍼(배열)에 대한 인덱스를 가진다.*/
	//ID3D11Buffer *m_pd3dIndexBuffer;
	////인덱스 버퍼가 포함하는 인덱스의 개수이다. 
	//UINT m_nIndices;
	////인덱스 버퍼에서 메쉬를 표현하기 위해 사용되는 시작 인덱스이다. 
	//UINT m_nStartIndex;
	////각 인덱스에 더해질 인덱스이다. 
	//int m_nBaseVertex;
	////--------------------------------------------------------------------------------
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D11Device *pd3dDevice);
	virtual ~CTriangleMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

	//05-1
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};

//06
class CCubeMesh : public CMesh
{
public:
	//072
	CCubeMesh(ID3D11Device *pd3dDevice, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//072
class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMesh();
};

class CHeightMapGridMesh : public CMesh
{
	//격자의 크기(가로: x방향, 세로: z-방향) 이다.
	int m_nWidth;
	int m_nLength;
	//격자의 스케일(가로:x-방향, 세로: z-방향, 높이 : y방향) 벡터이다. 
	//실제 격자 메쉬의 각 정점의 x좌표, y좌표, z좌표는
	//스케일 벡터의 x좌표, y좌표, z좌표로 곱한 값을 갖는다.
	//즉, 실제 격자의 x축 방향의 간격은 1이 아니라 스케일 벡터의 x좌표가 된다.
	//이렇게 하면 작은 격자를 사용하더라도 큰 격자를 생성할 수 있다.
	D3DXVECTOR3 m_d3dxvScale;

public:
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXCOLOR d3dxColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
};