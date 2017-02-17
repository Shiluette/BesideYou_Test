#include "stdafx.h"
#include "SkinnedMesh.h"



//1
int CAnimationController::s_nBoneCount = 14;
int CAnimationController::s_nAnimationClip = 1;
D3DXMATRIX* CAnimationController::s_pd3dxmtxBoneOffsets = NULL;
BoneAnimationData** CAnimationController::s_ppBoneAnimationData = NULL;
ID3D11Buffer* CAnimationController::s_pd3dcbBones = nullptr;

//1
CSkinnedMesh::CSkinnedMesh(ID3D11Device *pd3dDevice, char *pszFileName, int nBoneCount) : CMeshTexturedIlluminated(pd3dDevice)
{
	AllocConsole();

	freopen("CONOUT$", "wb", stdout);

	ifstream fin(pszFileName);
	string ignore;

	if (!fin.fail())
	{
		// 데이터를 읽어와 필요한 정점, 인덱스, 본, 애니메이션 수 파악
		fin >> ignore;//[FBX_META_DATA]
		fin >> ignore >> ignore;	//MeshCount	//띄어쓰기
		fin >> ignore;				//[MESH_DATA]
		fin >> ignore >> m_nVertices;	//VertexCount
		fin >> ignore >> m_nIndices;	
		fin >> ignore >> ignore;
		fin >> ignore >> ignore;

		// 정점 데이터를 저장
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
		m_pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

		if (nBoneCount)
		{
			m_pd3dxvBoneIndices = new D3DXVECTOR4[m_nVertices];
			m_pd3dxvBoneWeights = new D3DXVECTOR4[m_nVertices];
		}
		fin >> ignore;

		for (int i = 0; i < m_nVertices; i++)
		{
			fin >> ignore >> m_pd3dxvPositions[i].x >> m_pd3dxvPositions[i].y >> m_pd3dxvPositions[i].z;
			fin >> ignore >> m_pd3dxvNormals[i].x >> m_pd3dxvNormals[i].y >> m_pd3dxvNormals[i].z;
			fin >> ignore >> m_pd3dxvTexCoords[i].x >> m_pd3dxvTexCoords[i].y;
			if (nBoneCount)
			{
				fin >> ignore >> m_pd3dxvBoneIndices[i].x >> m_pd3dxvBoneIndices[i].y >> m_pd3dxvBoneIndices[i].z >> m_pd3dxvBoneIndices[i].w;
				fin >> ignore >> m_pd3dxvBoneWeights[i].x >> m_pd3dxvBoneWeights[i].y >> m_pd3dxvBoneWeights[i].z >> m_pd3dxvBoneWeights[i].w;
			}
		}

		/*for (int i = 0; i < m_nVertices; i++)
		D3DXVec3TransformNormal(&m_pd3dxvNormals[i], &m_pd3dxvNormals[i], &mtxRotate);
		*/

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

	//CreateRasterizerState(pd3dDevice);

	//2016.08.13 박종혁 상수버퍼 이하, 아래 내용은 컨트롤러로.
	//
}

//1
CSkinnedMesh::~CSkinnedMesh()
{
	if (m_pd3dWeightBuffer) m_pd3dWeightBuffer->Release();
	if (m_pd3dBoneIndiceBuffer) m_pd3dBoneIndiceBuffer->Release();

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pd3dxvNormals) delete[] m_pd3dxvNormals;
	if (m_pd3dxvTexCoords) delete[] m_pd3dxvTexCoords;
	if (m_pd3dxvBoneWeights) delete[] m_pd3dxvBoneWeights;
	if (m_pd3dxvBoneIndices) delete[] m_pd3dxvBoneIndices;

	FreeConsole();
}

//1
CAnimationController::CAnimationController(float fSize)
{
	m_fFBXModelScaleSize = fSize;
	m_fFBXAnimationTime = 0.0f;
	m_nFBXAnimationNum = 0;

	D3DXMatrixIdentity(&m_d3dxmtxLocalTransform);
	D3DXMATRIX mtxScale;
	D3DXMATRIX mtxRotate;
	D3DXMatrixScaling(&mtxScale, m_fFBXModelScaleSize, m_fFBXModelScaleSize, m_fFBXModelScaleSize);
	D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(-90.0f), (float)D3DXToRadian(-90.0f), (float)D3DXToRadian(-90.0f));
	m_d3dxmtxLocalTransform = mtxScale * mtxRotate;

	// (애니메이션을 포함한 메쉬일 경우) 본 정보와 애니메이션 정보 저장
	if (s_nBoneCount)
	{
		m_pd3dxmtxSQTTransform = new D3DXMATRIX[s_nBoneCount];
		m_pd3dxmtxFinalBone = new D3DXMATRIX[s_nBoneCount];
	}

	SetAnimationState(0);
	IdleSet();//애니메이션 데이터 초기화	
}

//1
CAnimationController::~CAnimationController()
{
	if (m_pd3dxmtxFinalBone) delete[] m_pd3dxmtxFinalBone;
	if (m_pd3dxmtxSQTTransform) delete[] m_pd3dxmtxSQTTransform;

}

//1
void CAnimationController::LoadAnimationSet(ID3D11Device* pd3dDevice)
{
	ifstream fin("../Data/BottomAnimation.txt");

	string ignore;
	fin >> ignore >> ignore >> ignore;//[AnimationData]

	s_pd3dxmtxBoneOffsets = new D3DXMATRIX[s_nBoneCount];
	// 뼈대 자체의 오프셋 행렬을 저장
	fin >> ignore; //[OFFSET_MATRIX]
	for (int i = 0; i < s_nBoneCount; i++)
		fin >> ignore >> s_pd3dxmtxBoneOffsets[i]._11 >> s_pd3dxmtxBoneOffsets[i]._12 >> s_pd3dxmtxBoneOffsets[i]._13 >> s_pd3dxmtxBoneOffsets[i]._14
		>> s_pd3dxmtxBoneOffsets[i]._21 >> s_pd3dxmtxBoneOffsets[i]._22 >> s_pd3dxmtxBoneOffsets[i]._23 >> s_pd3dxmtxBoneOffsets[i]._24
		>> s_pd3dxmtxBoneOffsets[i]._31 >> s_pd3dxmtxBoneOffsets[i]._32 >> s_pd3dxmtxBoneOffsets[i]._33 >> s_pd3dxmtxBoneOffsets[i]._34
		>> s_pd3dxmtxBoneOffsets[i]._41 >> s_pd3dxmtxBoneOffsets[i]._42 >> s_pd3dxmtxBoneOffsets[i]._43 >> s_pd3dxmtxBoneOffsets[i]._44;

	fin >> ignore;//[Animation_clip]

	// 여기에서부터 애니메이션을 담는다.
	s_ppBoneAnimationData = new BoneAnimationData*[s_nAnimationClip];
	BoneAnimationData *pBoneAnimationData;
	for (int k = 0; k < s_nAnimationClip; k++)
	{
		fin >> ignore >> ignore;//clip_name;
		pBoneAnimationData = new BoneAnimationData[s_nBoneCount];

		for (int i = 0; i < s_nBoneCount; i++)//bone 개수만큼
		{
			fin >> ignore >> ignore >> pBoneAnimationData[i].m_nFrameCount;//i번째 bone의 프레임카운터

			pBoneAnimationData[i].m_pd3dxvTranslate = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
			pBoneAnimationData[i].m_pd3dxvScale = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
			pBoneAnimationData[i].m_pd3dxvQuaternion = new D3DXVECTOR4[pBoneAnimationData[i].m_nFrameCount];
			pBoneAnimationData[i].m_pfAniTime = new float[pBoneAnimationData[i].m_nFrameCount];

			for (int j = 0; j < pBoneAnimationData[i].m_nFrameCount; j++)
			{
				fin >> ignore >> pBoneAnimationData[i].m_pfAniTime[j];

				fin >> ignore >> pBoneAnimationData[i].m_pd3dxvTranslate[j].x >> pBoneAnimationData[i].m_pd3dxvTranslate[j].y
					>> pBoneAnimationData[i].m_pd3dxvTranslate[j].z;
				fin >> ignore >> pBoneAnimationData[i].m_pd3dxvScale[j].x >> pBoneAnimationData[i].m_pd3dxvScale[j].y
					>> pBoneAnimationData[i].m_pd3dxvScale[j].z;
				fin >> ignore >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].x >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].y
					>> pBoneAnimationData[i].m_pd3dxvQuaternion[j].z >> pBoneAnimationData[i].m_pd3dxvQuaternion[j].w;
			}
		}
		s_ppBoneAnimationData[k] = pBoneAnimationData;
	}
	fin.close();

	CreateConstantBuffer(pd3dDevice);//이건 하나만 만들어도 될 것 같은데?? 2016.08.13 박종혁
}

//1
//컨트롤러 전용함수
void CAnimationController::CreateConstantBuffer(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_SKINNED);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &s_pd3dcbBones);
}

//1
void CAnimationController::IdleSet()
{
	m_nFBXAnimationNum = 0;// ANIM_STATIC;
	m_fFBXAnimationTime = 0.0f;

	m_nFBXStartFrameNum = 0;
	m_nFBXMaxFrameNum = 50;
	//m_nFBXStartFrameNum = 152;
	//m_nFBXMaxFrameNum = 206;
	m_nFBXNowFrameNum = m_nFBXStartFrameNum;
}
