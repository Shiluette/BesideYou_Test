#include "stdafx.h"

//3.3
CFBXMesh::CFBXMesh(ID3D11Device *pd3dDevice, char *pszFileName, float fSize) : CMeshTexturedIlluminated(pd3dDevice)
{
	m_fFBXModelSize = fSize;
	m_fFBXAnimationTime = 0.0f;
	m_nFBXAnimationNum = 0;
	m_nFBXNowFrameNum = 0;

	ifstream fin;

	int filename_length = strlen(pszFileName);
	char* ch = new char[200];
	char* sToken = new char[50];
	char* temp;
	fin.open(pszFileName);
	if (!fin.fail())
	{
		// �����͸� �о�� �ʿ��� ����, �ε���, ��, �ִϸ��̼� �� �ľ�
		fin.getline(ch, 200);
		fin.getline(ch, 200);
		m_nVertices = stoi(ch);
		fin.getline(ch, 200);
		m_nIndices = stoi(ch);
		fin.getline(ch, 200);
		m_nBoneCount = stoi(ch);
		fin.getline(ch, 200);
		m_nAnimationClip = stoi(ch);

		// ���� �����͸� ����
		fin.getline(ch, 200);
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
			// Vertice
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);	 m_pd3dxvPositions[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvPositions[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvPositions[i].z = stof(sToken);
			// Normal
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pd3dxvNormals[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvNormals[i].y = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvNormals[i].z = stof(sToken);
			// UV
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pd3dxvTexCoords[i].x = stof(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pd3dxvTexCoords[i].y = stof(sToken);
			if (m_nBoneCount)
			{			// BoneIndice
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp); 	 m_pd3dxvBoneIndices[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneIndices[i].w = stof(sToken);
				// BoneWeight
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);   m_pd3dxvBoneWeights[i].x = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].y = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].z = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxvBoneWeights[i].w = stof(sToken);
			}
		}

		/*for (int i = 0; i < m_nVertices; ++i)
		{
			m_pd3dxvPositions[i].x *= 0.3f;
			m_pd3dxvPositions[i].y *= 0.3f;
			m_pd3dxvPositions[i].z *= 0.3f;
		}*/

		// �ε��� ������ ����
		fin.getline(ch, 200);
		m_pnIndices = new UINT[m_nIndices];

		for (int i = 0; i < m_nIndices; i += 3)
		{
			fin.getline(ch, 200);
			sToken = strtok_s(ch, " ", &temp);   m_pnIndices[i] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pnIndices[i + 1] = stoi(sToken);
			sToken = strtok_s(NULL, " ", &temp); m_pnIndices[i + 2] = stoi(sToken);
		}

		// (�ִϸ��̼��� ������ �޽��� ���) �� ������ �ִϸ��̼� ���� ����
		if (m_nBoneCount)
		{
			fin.getline(ch, 200);
			m_pBoneHierarchy = new UINT[m_nBoneCount];
			m_pd3dxmtxBoneOffsets = new D3DXMATRIX[m_nBoneCount];
			m_pd3dxmtxSQTTransform = new D3DXMATRIX[m_nBoneCount];
			m_pd3dxmtxFinalBone = new D3DXMATRIX[m_nBoneCount];

			// �θ� ���븦 ����Ű�� BoneHierarchy�� ����
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				m_pBoneHierarchy[i] = stoi(ch);
			}
			// ���� ��ü�� ������ ����� ����
			fin.getline(ch, 200);
			for (int i = 0; i < m_nBoneCount; i++)
			{
				fin.getline(ch, 200);
				sToken = strtok_s(ch, " ", &temp);   m_pd3dxmtxBoneOffsets[i]._11 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._12 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._13 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._14 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._21 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._22 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._23 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._24 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._31 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._32 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._33 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._34 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._41 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._42 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._43 = stof(sToken);
				sToken = strtok_s(NULL, " ", &temp); m_pd3dxmtxBoneOffsets[i]._44 = stof(sToken);
			}

			// ���⿡������ �ִϸ��̼��� ��´�.
			m_ppBoneAnimationData = new BoneAnimationData*[m_nAnimationClip];

			BoneAnimationData *pBoneAnimationData;
			for (int k = 0; k < m_nAnimationClip; k++)
			{
				pBoneAnimationData = new BoneAnimationData[m_nBoneCount];

				fin.getline(ch, 200);
				for (int i = 0; i < m_nBoneCount; i++)
				{
					fin.getline(ch, 200);
					sToken = strtok_s(ch, " ", &temp);
					sToken = strtok_s(NULL, " ", &temp);
					pBoneAnimationData[i].m_nFrameCount = stoi(sToken);
					pBoneAnimationData[i].m_pd3dxvTranslate = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvScale = new D3DXVECTOR3[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pd3dxvQuaternion = new D3DXVECTOR4[pBoneAnimationData[i].m_nFrameCount];
					pBoneAnimationData[i].m_pfAniTime = new float[pBoneAnimationData[i].m_nFrameCount];
					for (int j = 0; j < pBoneAnimationData[i].m_nFrameCount; j++)
					{
						fin.getline(ch, 200);
						sToken = strtok_s(ch, " ", &temp);
						pBoneAnimationData[i].m_pfAniTime[j] = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvTranslate[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvScale[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].x = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].y = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].z = stof(sToken);
						sToken = strtok_s(NULL, " ", &temp);
						pBoneAnimationData[i].m_pd3dxvQuaternion[j].w = stof(sToken);
					}
				}
				m_ppBoneAnimationData[k] = pBoneAnimationData;
			}
		}
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

	CreateRasterizerState(pd3dDevice);
	CreateConstantBuffer(pd3dDevice);

	// idle ������ ������ ���� üũ.
	if (m_nBoneCount)
		m_nFBXMaxFrameNum = m_ppBoneAnimationData[0][0].m_nFrameCount - 1;
}

//3.3
CFBXMesh::~CFBXMesh()
{
	if (m_pd3dWeightBuffer) m_pd3dWeightBuffer->Release();
	if (m_pd3dBoneIndiceBuffer) m_pd3dBoneIndiceBuffer->Release();
	if (m_pd3dcbBones) m_pd3dcbBones->Release();

	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pd3dxvNormals) delete[] m_pd3dxvNormals;
	if (m_pd3dxvTexCoords) delete[] m_pd3dxvTexCoords;
	if (m_pd3dxvBoneWeights) delete[] m_pd3dxvBoneWeights;
	if (m_pd3dxvBoneIndices) delete[] m_pd3dxvBoneIndices;
	if (m_ppBoneAnimationData) delete[] m_ppBoneAnimationData;
	if (m_pd3dxmtxFinalBone) delete[] m_pd3dxmtxFinalBone;
	if (m_pd3dxmtxSQTTransform) delete[] m_pd3dxmtxSQTTransform;
	if (m_pBoneHierarchy) delete[] m_pBoneHierarchy;
	if (m_pd3dxmtxBoneOffsets) delete[] m_pd3dxmtxBoneOffsets;

}

//3.3
void CFBXMesh::UpdateBoneTransform(ID3D11DeviceContext *pd3dDeviceContext, int nAnimationNum, int nNowFrame)
{
	for (int i = 0; i < m_nBoneCount; i++)
	{
		MakeBoneMatrix(nNowFrame, nAnimationNum, i, *(m_pd3dxmtxSQTTransform + i));
	}


	// ���������� ���� �⺻ ����������� �����־� ���� ����� ������ش�.
	for (int i = 0; i < m_nBoneCount; i++)
	{
		D3DXMATRIX offset = m_pd3dxmtxBoneOffsets[i];
		D3DXMATRIX toRoot = m_pd3dxmtxSQTTransform[i];
		D3DXMatrixMultiply(&m_pd3dxmtxFinalBone[i], &offset, &toRoot);
	}

	// ������۷� ���� ��İ��� �Ѱ�����.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbBones, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_SKINNED *pcbBones = (VS_CB_SKINNED*)d3dMappedResource.pData;
	for (int i = 0; i < m_nBoneCount; i++)
	{
		D3DXMatrixTranspose(&pcbBones->m_d3dxmtxBone[i], &m_pd3dxmtxFinalBone[i]);
	}
	pd3dDeviceContext->Unmap(m_pd3dcbBones, 0);

	//��� ���۸� ����(VS_SLOT_SKINNEDBONE)�� �����Ѵ�.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_SKINNEDBONE, 1, &m_pd3dcbBones);
}

//3.3
void CFBXMesh::MakeBoneMatrix(int nNowframe, int nAnimationNum, int nBoneNum, D3DXMATRIX& BoneMatrix)
{
	// XMAffine �Լ������� scale�� VECTOR3�� ������
	// D3DXAffine �Լ������� scale�� ����� ����Ѵ�.
	if (m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_nFrameCount != 0)
	{
		float fScale = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvScale[nNowframe].z;
		D3DXVECTOR3 d3dxvTranslate = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvTranslate[nNowframe];
		D3DXQUATERNION d3dxvQuaternion = m_ppBoneAnimationData[nAnimationNum][nBoneNum].m_pd3dxvQuaternion[nNowframe];
		D3DXVECTOR3 d3dxvZero = { 0.0f, 0.0f, 0.0f };

		D3DXMatrixAffineTransformation(&BoneMatrix, fScale, &d3dxvZero, &d3dxvQuaternion, &d3dxvTranslate);
	}
	else // �ش� ���� �ִϸ��̼� �������� �ƿ� ���� ��� ��������� ��������.
	{
		D3DXMatrixIdentity(&BoneMatrix);
	}
}

void CFBXMesh::CreateConstantBuffer(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_SKINNED);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbBones);
}

//3.3
// ���� �ִϸ��̼��� ���� ���������� �ѱ��.
// �߰����� �ִϸ��̼� ������ ���� ������ �������� ��� true�� �����Ѵ�.
bool CFBXMesh::FBXFrameAdvance(float fTimeElapsed)
{
	m_fFBXAnimationTime += fTimeElapsed;

	if (m_fFBXAnimationTime > ANIFRAMETIME)	// 0.0333333f�ʰ� ������ 1������ �ø���.
	{
		if (m_nFBXNowFrameNum < m_nFBXMaxFrameNum - 1)
		{
			m_nFBXNowFrameNum++;
			m_fFBXAnimationTime = 0.0f;
			return false;
		}
		else
		{
			m_nFBXNowFrameNum = 0;
			m_fFBXAnimationTime = 0.0f;
			return true;
		}
	}
	else
		return false;
}

void CFBXMesh::SetAnimation(int nFBXAnimation)
{
	m_nFBXAnimationNum = nFBXAnimation;
	m_nFBXNowFrameNum = 0;
	m_nFBXMaxFrameNum = m_ppBoneAnimationData[nFBXAnimation][0].m_nFrameCount;
	m_fFBXAnimationTime = 0.0f;
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
	//����
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
	//	D3DXMatrixTranslation(&mtxTranslate, -150.0f, 0.0f, 200.0f);//���� �߸�, z�� ����, ���� �պ��� �տ� ������, x�� ����
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

	//���ؽ� ���� �ʱ�ȭ
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

