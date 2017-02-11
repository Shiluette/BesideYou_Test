#pragma once
#include "Mesh.h"

//1
struct VS_CB_SKINNED
{
	D3DXMATRIX	m_d3dxmtxBone[30];
};//�̰�.. �޽��� �ƴ϶� ������Ʈ���� ��߰ڴ�.

//1
struct BoneAnimationData
{
	int m_nFrameCount;//�ʿ��Ѱ�? �ʿ��ϴ�
	float	*m_pfAniTime;//���ʿ��ϴ�. �㳪 ���� ��
	D3DXVECTOR3 *m_pd3dxvScale;
	D3DXVECTOR3 *m_pd3dxvTranslate;
	D3DXVECTOR4 *m_pd3dxvQuaternion;
};

//1
class CSkinnedMesh : public CMeshTexturedIlluminated
{
private:
	D3DXVECTOR3 *m_pd3dxvPositions;//�ʿ�
	D3DXVECTOR3	*m_pd3dxvNormals;//�ʿ�
	D3DXVECTOR2 *m_pd3dxvTexCoords;//�ʿ�
	D3DXVECTOR4 *m_pd3dxvBoneWeights;//������ �ʿ��� ������
	D3DXVECTOR4 *m_pd3dxvBoneIndices;//������ �ʿ��� ������

	// i�� ������ �θ� ����(parentIndex)�� ��´�.
	// i�� ����� �ִϸ��̼� Ŭ���� i��° BoneAnimation �ν��Ͻ��� ����.
	UINT *m_pBoneHierarchy;

	ID3D11Buffer *m_pd3dWeightBuffer;//���������� �ʿ��� ����ġ ����
	ID3D11Buffer *m_pd3dBoneIndiceBuffer;//���������� �ʿ��� �� �ε��� ����

public:
	CSkinnedMesh(ID3D11Device *pd3dDevice, char *pszFileName, int nBoneCount);
	virtual ~CSkinnedMesh();

	// �ش� �������� SR(Q)T ȸ���� �ݿ��� ����� ��ȯ

	//void SetFBXAnimationTime(float fFBXAnimationTime) { m_fFBXAnimationTime = fFBXAnimationTime; }
};

//1
class CAnimationController
{
	static BoneAnimationData **s_ppBoneAnimationData;//�̰� static���� ������ �� �ִ� ������
	static int s_nBoneCount;//18
	static int s_nAnimationClip;//2
	static D3DXMATRIX *s_pd3dxmtxBoneOffsets;
	// ���� �������
	static ID3D11Buffer *s_pd3dcbBones;//�ʿ��ϴ�

	D3DXMATRIX *m_pd3dxmtxSQTTransform;//�̰͵� �ʿ��ϴ�.
	D3DXMATRIX *m_pd3dxmtxFinalBone;//������� �迭(�̰� �ʿ��ϴ�.)
	D3DXMATRIX m_d3dxmtxLocalTransform;//���������� �ʿ��� ���� ��ȯ ���

	float m_fFBXAnimationTime;	// ���� AnimationTime (���������� 0.0333333f �� ����)	
	int m_nFBXAnimationNum;		// ���� ������ �ִϸ��̼��� Set�ѹ� 0 Ȥ�� 1�̴�.
	float m_fFBXModelScaleSize;		// ���� ������ (���� Animate���� �������ֱ� ����)	

	unsigned int m_uiAnimationState;

	int m_nFBXStartFrameNum;	// ���� �ִϸ��̼��� �����ϴ� ������ �ѹ�
	int m_nFBXMaxFrameNum;		// ���� ������ �ִϸ��̼��� �ִ� ������ ��.
	int m_nFBXNowFrameNum;		// ���� �������� �ִϸ��̼��� ���� ������ ��.

public:
	CAnimationController(float fSize);
	~CAnimationController();

	static void LoadAnimationSet(ID3D11Device* pd3dDevice);
	static void CreateConstantBuffer(ID3D11Device *pd3dDevice);

	unsigned int GetAnimationState() { return m_uiAnimationState; }
	void SetAnimationState(unsigned int CurrentState)
	{
		m_uiAnimationState = CurrentState;
	}
	void IdleSet();
};