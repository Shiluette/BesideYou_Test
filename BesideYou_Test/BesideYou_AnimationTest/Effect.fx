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

////2
//cbuffer cbSkinned : register(b10)
//{
//	matrix gBoneTransform[30];
//}
//----------- 상수 버퍼 -----------//






//---------------------------------//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

Texture2D gtxtDetailTexture : register(t1);
SamplerState gDetailSamplerState : register(s1);
//---------------------------------//








//----------------------------------------//
//VS,PS
//사용 : 플레이어캐릭터
//사용중

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};


struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//입력되는 정점의 색상을 그대로 출력한다. 
	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//입력되는 정점의 색상을 그대로 출력한다. 
}

//----------------------------------------//






//----------------------------------------//
//VSDiffusedColor,PSDiffusedColor
//사용 : 터레인
//사용안함
//첫번째 쉐이더							  
// 정점이 색상을 갖는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_DIFFUSED_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR0;
};

// 정점이 색상을 갖는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_DIFFUSED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

VS_DIFFUSED_COLOR_OUTPUT VSDiffusedColor(VS_DIFFUSED_COLOR_INPUT input)
{
	VS_DIFFUSED_COLOR_OUTPUT output = (VS_DIFFUSED_COLOR_OUTPUT)0;
	output.position = mul(float4(input.position, 1.0f), mul(mul(gmtxWorld, gmtxView), gmtxProjection));
	output.color = input.color;

	return(output);
}

float4 PSDiffusedColor(VS_DIFFUSED_COLOR_OUTPUT input) : SV_Target
{
	return(input.color);
}	  
//----------------------------------------//







//----------------------------------------//
//VSInstancedLightingColor,PSInstancedLightingColor
//사용 : 객체 인스턴싱 + 조명효과
//사용안함
//두번째, 세번째, 네번째 쉐이더

// 인스턴싱을 하면서 조명을 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_INSTANCED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4x4 mtxTransform : INSTANCEPOS;
};

// 인스턴싱을 하면서 조명을 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_INSTANCED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

VS_INSTANCED_LIGHTING_COLOR_OUTPUT VSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

float4 PSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
float4 cIllumination = Lighting(input.positionW, input.normalW);		// 월드변환된 위치정보와 노멀정보를 통해 Lighting 계산

return(cIllumination);
}
//----------------------------------------//








//----------------------------------------//
//VSLightingColor,PSLightingColor
//사용 : 조명
//사용안함

// 조명을 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

// 조명을 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	// 월드좌표계에서 정점의 위치와 법선 벡터를 나타낸다.
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//조명의 영향을 계산하기 위한 정점의 법선 벡터와 정점의 위치를 계산하는 정점 쉐이더 함수이다.
VS_LIGHTING_COLOR_OUTPUT VSLightingColor(VS_LIGHTING_COLOR_INPUT input)
{
	VS_LIGHTING_COLOR_OUTPUT output = (VS_LIGHTING_COLOR_OUTPUT)0;
	//조명의 영향을 계산하기 위하여 월드좌표계에서 정점의 위치와 법선 벡터를 구한다.
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

// 각 픽셀에 대하여 조명의 영향을 반영한 색상을 계산하기 위한 픽셀 쉐이더 함수이다.
float4 PSLightingColor(VS_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

	return(cIllumination);
}
//----------------------------------------//








////-------------------------------------//
////-------- 왜 있는거지 이건(x) --------//

//// 인스턴싱을 하면서 정점이 색상을 갖는 경우 정점 쉐이더의 입력을 위한 구조체이다.
//struct VS_INSTANCED_DIFFUSED_COLOR_INPUT
//{
//	float3 position : POSITION;
//	float4 color : COLOR0;
//	float4x4 mtxTransform : INSTANCEPOS;
//};
//// 인스턴싱을 하면서 정점이 색상을 갖는 경우 정점 쉐이더의 출력을 위한 구조체이다.
//struct VS_INSTANCED_DIFFUSED_COLOR_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float4 color : COLOR0;
//};
//
//VS_INSTANCED_DIFFUSED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_DIFFUSED_COLOR_INPUT input)
//{
//	VS_INSTANCED_DIFFUSED_COLOR_OUTPUT output = (VS_INSTANCED_DIFFUSED_COLOR_OUTPUT)0;
//	output.position = mul(mul(mul(float4(input.position, 1.0f), input.mtxTransform), gmtxView), gmtxProjection);
//	output.color = input.color;
//
//	return(output);
//}
//
//float4 PSInstancedDiffusedColor(VS_INSTANCED_DIFFUSED_COLOR_OUTPUT input) : SV_Target
//{
//	return(input.color);
//}

////-------- 왜 있는거지 이건(x) --------//
////-------------------------------------//







////----------------------------------------//
////---------- 인스턴싱 쉐이더(x) ----------//

//// 인스턴싱을 위한 쉐이더 변수 구조체
//struct VS_INSTANCED_COLOR_INPUT
//{
//	float3 position : POSITION;
//	float4 color : COLOR0;
//	column_major float4x4 mtxTransform : INSTANCEPOS;
//	float4 instanceColor : INSTANCECOLOR;
//};
//
//struct VS_INSTANCED_COLOR_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float4 color : COLOR0;
//	//시스템 생성 변수로 정점 쉐이더에 전달되는 객체 인스턴스의 ID를 픽셀 쉐이더로 전달한다.
//	float4 instanceID : INDEX;
//};
//
//// 인스턴싱을 위한 정점 쉐이더.
//VS_INSTANCED_COLOR_OUTPUT VSInstancedDiffusedColor(VS_INSTANCED_COLOR_INPUT input, uint instanceID : SV_InstanceID)
//{
//	VS_INSTANCED_COLOR_OUTPUT output = (VS_INSTANCED_COLOR_OUTPUT)0;
//	output.position = mul(float4(input.position, 1), input.mtxTransform);
//	output.position = mul(output.position, gmtxView);
//	output.position = mul(output.position, gmtxProjection);
//	output.color = (instanceID % 5) ? input.color : (input.color * 0.3f + input.instanceColor * 0.7f);
//	output.instanceID = instanceID;
//	return output;
//}
//
//float4 PSInstancedDiffusedColor(VS_INSTANCED_COLOR_OUTPUT input) : SV_Target
//{
//	return input.color;
//}
////---------- 인스턴싱 쉐이더(x) ----------//
////----------------------------------------//






//------------------------------------------//
//VSTexturedColor,PSTexturedColor
//사용 : SkyBox
//사용중

struct VS_TEXTURED_COLOR_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

//텍스쳐를 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_TEXTURED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

VS_TEXTURED_COLOR_OUTPUT VSTexturedColor(VS_TEXTURED_COLOR_INPUT input)
{
	VS_TEXTURED_COLOR_OUTPUT output = (VS_TEXTURED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

//각 픽셀에 대하여 텍스쳐 샘플링을 하기 위한 픽셀 쉐이더 함수이다.
float4 PSTexturedColor(VS_TEXTURED_COLOR_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord);

	return(cColor);
}
//------------------------------------------//






//------------------------------------------//
//VSInstancedTexturedColor
//사용 : 텍스쳐가 있는 객체 인스턴싱
//사용중

//인스턴싱을 하면서 텍스쳐를 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_COLOR_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4x4 mtxTransform : INSTANCEPOS;
};

//인스턴싱을 하면서 텍스쳐를 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};


VS_INSTANCED_TEXTURED_COLOR_OUTPUT VSInstancedTexturedColor(VS_INSTANCED_TEXTURED_COLOR_INPUT input)
{
	VS_INSTANCED_TEXTURED_COLOR_OUTPUT output = (VS_INSTANCED_TEXTURED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), input.mtxTransform), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

float4 PSInstancedTexturedColor(VS_INSTANCED_TEXTURED_COLOR_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord);

	return(cColor);
}
//------------------------------------------//






//------------------------------------------//
//VSDetailTexturedColor,PSDetailTexturedColor
//사용 : 디테일텍스쳐
//사용안함

//131
//디테일 텍스쳐를 사용하는 경우 정점 쉐이더의 입력과 출력을 위한 구조체이다.
struct VS_DETAIL_TEXTURED_COLOR_INPUT
{
	float3 position : POSITION;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;
};

//131
struct VS_DETAIL_TEXTURED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;
};



//131
VS_DETAIL_TEXTURED_COLOR_OUTPUT VSDetailTexturedColor(VS_DETAIL_TEXTURED_COLOR_INPUT input)
{
	VS_DETAIL_TEXTURED_COLOR_OUTPUT output = (VS_DETAIL_TEXTURED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.texCoordBase = input.texCoordBase;
	output.texCoordDetail = input.texCoordDetail;

	return(output);
}

//131
float4 PSDetailTexturedColor(VS_DETAIL_TEXTURED_COLOR_OUTPUT input) : SV_Target
{
	float4 cBaseTexColor = gtxtTexture.Sample(gSamplerState, input.texCoordBase);
	float4 cDetailTexColor = gtxtDetailTexture.Sample(gDetailSamplerState, input.texCoordDetail);
	float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

	return(cColor);
}
//------------------------------------------//







//------------------------------------------//
//VSTexturedLightingColor, PSTexturedLightingColor
//사용안함

//133
//텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//133
//텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
};


//133
VS_TEXTURED_LIGHTING_COLOR_OUTPUT VSTexturedLightingColor(VS_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

//133
float4 PSTexturedLightingColor(VS_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
float4 cIllumination = Lighting(input.positionW, input.normalW);
float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;

return(cColor);
}
//------------------------------------------//








//-------------------------------------------//
//VSDetailTexturedLightingColor,PSDetailTexturedLightingColor
//Terrian
//사용중

//133
//디테일 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;
};

//133
//디테일 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoordBase : TEXCOORD0;
	float2 texCoordDetail : TEXCOORD1;
};

//133
VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT VSDetailTexturedLightingColor(VS_DETAIL_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoordBase = input.texCoordBase;
	output.texCoordDetail = input.texCoordDetail;

	return(output);
}

//133
float4 PSDetailTexturedLightingColor(VS_DETAIL_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
float4 cIllumination = Lighting(input.positionW, input.normalW);
float4 cBaseTexColor = gtxtTexture.Sample(gSamplerState, input.texCoordBase);
float4 cDetailTexColor = gtxtDetailTexture.Sample(gDetailSamplerState, input.texCoordDetail);
float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

return(cColor*cIllumination);
}
//-----------------------------------------------//








//-----------------------------------------------//
//VSInstancedTexturedLightingColor,PSInstancedTexturedLightingColor
//사용안함

//133
//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 입력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4x4 mtxTransform : INSTANCEPOS;
};

//133
//인스턴싱, 텍스쳐와 조명을 같이 사용하는 경우 정점 쉐이더의 출력을 위한 구조체이다.
struct VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//133
VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT VSInstancedTexturedLightingColor(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.texCoord = input.texCoord;

	return(output);
}

//133  
float4 PSInstancedTexturedLightingColor(VS_INSTANCED_TEXTURED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.texCoord) * cIllumination;

	return(cColor);
}
//--------------------------------------------------//




////2
//struct VS_SKINNED_INPUT
//{
//	float3 position : POSITION;
//	float3 normal : NORMAL;
//	float2 texCoord : TEXCOORD0;
//	float4 weights : WEIGHTS;
//	float4 boneindices : BONEINDICES;
//};
//
////2
//struct VS_SKINNED_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float3 positionW : POSITION;
//	float3 normalW : NORMAL;
//	float2 texCoord : TEXCOORD0;
//};
//
////2
//VS_SKINNED_OUTPUT VSSkinned(VS_SKINNED_INPUT input)
//{
//	VS_SKINNED_OUTPUT output = (VS_SKINNED_OUTPUT)0;
//
//	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	weights[0] = input.weights.x;
//	weights[1] = input.weights.y;
//	weights[2] = input.weights.z;
//	//weights[3] = input.weights.w;
//	weights[3] = 1 - weights[0] - weights[1] - weights[2];
//
//	float3 posL = float3(0.0f, 0.0f, 0.0f);
//	float3 normalL = float3(0.0f, 0.0f, 0.0f);
//
//	for (int i = 0; i < 4; i++)
//	{
//		posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
//		//posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]);
//		normalL += weights[i] * mul(input.normal, (float3x3)gBoneTransform[input.boneindices[i]]);
//	}
//	//박종혁
//	output.positionW = mul(mul(float4(posL, 1.0f), gBoneTransform[18]), gmtxWorld).xyz;
//
//	//output.positionW = mul(float4(posL, 1.0f), gmtxWorld).xyz;
//	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//	output.normalW = mul(normalL, (float3x3)gmtxWorld);
//	output.texCoord = input.texCoord;
//
//	return output;
//}
//
////2
//float4 PSSkinned(VS_SKINNED_OUTPUT input) : SV_Target
//{
//	input.normalW = normalize(input.normalW);
//	float4 cIllumination = Lighting(input.positionW, input.normalW);
//	float4 cColor = gtxtDefault.Sample(gssDefault, input.texCoord) * cIllumination;
//
//	return(cColor);
//}