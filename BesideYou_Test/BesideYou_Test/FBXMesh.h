#pragma once

//3.2
// 상수 버퍼를 위한 구조체
struct VS_CB_SKINNED
{
	D3DXMATRIX	m_d3dxmtxBone[30];
};

//3.2
struct BoneAnimationData
{
	int m_nFrameCount;//필요한가? 필요하다
	float	*m_pfAniTime;//불필요하다. 허나 아직 모름
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

	float m_fFBXModelSize;		// 모델의 사이즈 (보통 Animate에서 조절해주기 위함)
	float m_fFBXAnimationTime;	// 모델의 AnimationTime (공통적으로 0.0333333f 씩 증가)
	int m_nFBXAnimationNum;		// 모델이 실행할 애니메이션의 값을 관리한다.
	int m_nFBXMaxFrameNum;		// 모델이 실행할 애니메이션의 최대 프레임 수.
	int m_nFBXNowFrameNum;		// 모델이 진행중인 애니메이션의 현재 프레임 값.
public:
	CFBXMesh(ID3D11Device *pd3dDevice, char *pszFileName, float fSize);
	virtual ~CFBXMesh();
};

//3.2
//애니메이션이 없는 데이터
class CAssetMesh : public CMeshTexturedIlluminated
{
	unsigned int m_uitype;//캐릭터에 종속되는 메쉬타입일 경우, 손목에 무기를 장착시키기 위해 일정한 offset이 필요하다.
public:
	enum {
		Common = 0,
		CharacterWeapon = 1
	};
	CAssetMesh(ID3D11Device *pd3dDevice, const std::string& filename, unsigned int type = CAssetMesh::Common);
	virtual ~CAssetMesh();
};

