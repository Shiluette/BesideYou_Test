#pragma once

//3.2
// ��� ���۸� ���� ����ü
struct VS_CB_SKINNED
{
	D3DXMATRIX	m_d3dxmtxBone[30];
};

//3.2
struct BoneAnimationData
{
	int m_nFrameCount;//�ʿ��Ѱ�? �ʿ��ϴ�
	float	*m_pfAniTime;//���ʿ��ϴ�. �㳪 ���� ��
	D3DXVECTOR3 *m_pd3dxvScale;
	D3DXVECTOR3 *m_pd3dxvTranslate;
	D3DXVECTOR4 *m_pd3dxvQuaternion;
};

//3.2
class CFBXMesh : public CMeshTexturedIlluminated
{
private:
	D3DXVECTOR3 *m_pd3dxvPositions;
	D3DXVECTOR3	*m_pd3dxvNormals;
	D3DXVECTOR2 *m_pd3dxvTexCoords;
	D3DXVECTOR4 *m_pd3dxvBoneWeights;
	D3DXVECTOR4 *m_pd3dxvBoneIndices;

	ID3D11Buffer *m_pd3dWeightBuffer;
	ID3D11Buffer *m_pd3dBoneIndiceBuffer;

	int m_nBoneCount;
	int m_nAnimationClip;

	float m_fFBXModelSize;		// ���� ������ (���� Animate���� �������ֱ� ����)
	float m_fFBXAnimationTime;	// ���� AnimationTime (���������� 0.0333333f �� ����)
	int m_nFBXAnimationNum;		// ���� ������ �ִϸ��̼��� ���� �����Ѵ�.
	int m_nFBXMaxFrameNum;		// ���� ������ �ִϸ��̼��� �ִ� ������ ��.
	int m_nFBXNowFrameNum;		// ���� �������� �ִϸ��̼��� ���� ������ ��.
public:
	CFBXMesh(ID3D11Device *pd3dDevice, char *pszFileName, float fSize);
	virtual ~CFBXMesh();
};

//3.2
//�ִϸ��̼��� ���� ������
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

