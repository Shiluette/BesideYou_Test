#include "Light.fx"

//----------- 상수 버퍼 -----------//
//VS(slot0)
cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//VS(slot1)
cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

////PS(slot0)
//cbuffer cbColor : register(b0)
//{
//	float4	gcColor : packoffset(c0);
//};


//----------- 상수 버퍼 -----------//


//---------------------------------//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

Texture2D gtxtDetailTexture : register(t1);
SamplerState gDetailSamplerState : register(s1);

Texture2D gtxtDefault : register(t0);
SamplerState gssDefault : register(s0);
//---------------------------------//

//2
struct VS_SKINNED_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//2
float4 PSSkinned(VS_SKINNED_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtDefault.Sample(gssDefault, input.texCoord) * cIllumination;

	return(cColor);
}

