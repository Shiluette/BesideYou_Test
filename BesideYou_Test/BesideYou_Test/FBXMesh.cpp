#include "stdafx.h"


//3.2
CFBXMesh::CFBXMesh(ID3D11Device *pd3dDevice, char *pszFileName, float fSize) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_fFBXModelSize = fSize;
	m_fFBXAnimationTime = 0.0f;
	m_nFBXAnimationNum = 0;

	ifstream fin;
	string ignore;

	int filename_length = strlen(pszFileName);
	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;

	fin.open(pszFileName);

	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin >> ignore;//[FBX_META_DATA]
		fin >> ignore >> ignore;	//MeshCount	//띄어쓰기
		fin >> ignore;				//[MESH_DATA]
		fin >> ignore >> m_nVertices;	//VertexCount
		fin >> ignore >> m_nIndices;	//index
		fin >> ignore >> m_nBoneCount;	//BoneCount
		fin >> ignore >> m_nAnimationClip;	//애니메이션클립

		// 정점 데이터를 저장
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

		if (m_nBoneCount)
		{
			m_pd3dxvBoneIndices = new D3DXVECTOR4[m_nVertices];
			m_pd3dxvBoneWeights = new D3DXVECTOR4[m_nVertices];
		}

		for (int i = 0; i < m_nVertices; i++)
		{
			fin >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
			fin >> ignore >> m_pd3dxvNormals[i].x >> m_pd3dxvNormals[i].y >> m_pd3dxvNormals[i].z;
			fin >> ignore >> m_pd3dxvTexCoords[i].x >> m_pd3dxvTexCoords[i].y;
			if (m_nBoneCount)
			{
				fin >> ignore >> m_pd3dxvBoneIndices[i].x >> m_pd3dxvBoneIndices[i].y >> m_pd3dxvBoneIndices[i].z >> m_pd3dxvBoneIndices[i].w;
				fin >> ignore >> m_pd3dxvBoneWeights[i].x >> m_pd3dxvBoneWeights[i].y >> m_pd3dxvBoneWeights[i].z >> m_pd3dxvBoneWeights[i].w;
			}
		}

		m_pnIndices = new UINT[m_nIndices];
		fin >> ignore;//[INDEX_DATA]
		for (UINT i = 0; i < m_nIndices; ++i)
			fin >> m_pnIndices[i];
	}
	fin.close();

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvPositions, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, m_pd3dxvNormals, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, m_pd3dxvTexCoords, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dWeightBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR4), m_nVertices, m_pd3dxvBoneWeights, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dBoneIndiceBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR4), m_nVertices, m_pd3dxvBoneIndices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	ID3D11Buffer *pd3dBuffers[5] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer, m_pd3dWeightBuffer, m_pd3dBoneIndiceBuffer };
	UINT pnBufferStrides[5] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR4), sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[5] = { 0, 0, 0, 0, 0 };
	AssembleToVertexBuffer(5, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, m_pnIndices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
}

//3.2
CFBXMesh::~CFBXMesh()
{
	if (m_pd3dWeightBuffer) m_pd3dWeightBuffer->Release();
	if (m_pd3dBoneIndiceBuffer) m_pd3dBoneIndiceBuffer->Release();

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pd3dxvNormals) delete[] m_pd3dxvNormals;
	if (m_pd3dxvTexCoords) delete[] m_pd3dxvTexCoords;
	if (m_pd3dxvBoneWeights) delete[] m_pd3dxvBoneWeights;
	if (m_pd3dxvBoneIndices) delete[] m_pd3dxvBoneIndices;
}

//3.2
CAssetMesh::CAssetMesh(ID3D11Device *pd3dDevice, const std::string& filename, unsigned int type) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_uitype = type;
	std::ifstream in(filename);
	std::string ignore;

	in >> ignore//[FBX_META_DATA]
		>> ignore//MeshCOUNT:
		>> ignore;//1
	in >> ignore;//MESH_DATA
	in >> ignore >> m_nVertices;//VertexCount	
	in >> ignore >> m_nIndices;//IndexCount;
	in >> ignore >> ignore;//BoneCount;
	in >> ignore >> ignore;//AnimationClips;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2* pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	m_pnIndices = new UINT[m_nIndices];
	//생성
	in >> ignore;
	for (int i = 0; i < m_nVertices; ++i)
	{
		in >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
		in >> ignore >> pd3dxvNormals[i].x >> pd3dxvNormals[i].y >> pd3dxvNormals[i].z;
		in >> ignore >> pd3dxvTexCoords[i].x >> pd3dxvTexCoords[i].y;
	}
	in >> ignore;

	/*for (int i = 0; i < m_nVertices; ++i)
	{
		m_pd3dxvPositions[i].x *= 2.0f;
		m_pd3dxvPositions[i].y *= 2.0f;
		m_pd3dxvPositions[i].z *= 2.0f;

	}*/

	//if (m_uitype == CAssetMesh::CharacterWeapon)
	//{
	//	D3DXMATRIX mtxTranslate;
	//	D3DXMATRIX mtxRotate;
	//	D3DXMatrixTranslation(&mtxTranslate, -150.0f, 0.0f, 200.0f);//총이 뜨면, z를 줄임, 총이 손보다 앞에 있으면, x를 줄임
	//	D3DXMatrixRotationX(&mtxRotate, -90.0f);
	//	mtxRotate *= mtxTranslate;
	//	for (int i = 0; i < m_nVertices; i++)
	//		D3DXVec3TransformCoord(&m_pd3dxvPositions[i], &m_pd3dxvPositions[i], &mtxRotate);
	//}

	for (unsigned int i = 0; i < m_nIndices; i++)
	{
		in >> m_pnIndices[i];
	}
	in.close();//file close
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float previousY = 0;
	float previousZ = 0;

	//버텍스 버퍼 초기화
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;//Buffer ByteWidth
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pd3dxvNormals;//Sysmem pointer.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2)* m_nVertices;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	//CalculateBoundingCube();

	delete[] pd3dxvNormals;
	delete[] pd3dxvTexCoords;
}

//3.2
CAssetMesh::~CAssetMesh()
{
}