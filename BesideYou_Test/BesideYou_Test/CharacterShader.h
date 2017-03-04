#pragma once

//3.3
class CCharacterShader : public CIlluminatedShader
{
private:
	CMaterial *m_pMaterial;
	CTexture *m_pTexture;
public:
	CCharacterShader(int nObjects = 1);
	~CCharacterShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
};