#pragma once

#include "targetver.h"

// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <d3d11.h>
#include <d3dx11.h>
#include <mmsystem.h>
#include <math.h>
#include <d3dcompiler.h>  	//쉐이더 컴파일 함수를 사용하기 위한 헤더 파일
#include <D3DX10Math.h>	//Direct3D 수학 함수를 사용하기 위한 헤더 파일
#include <D3D9Types.h>

//다이렉트2d
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <DxErr.h>

//--
#include "BesideYou_Test.h"
#include "Timer.h"

#include "Mesh.h"
#include "MeshIlluminated.h"

#include "Camera.h"
#include "Object.h"
#include "Player.h"

#include "Shader.h"
#include "IlluminatedShader.h"

#include "Scene.h"
#include "GameFramework.h"
//--

//---------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#define FRAME_BUFFER_WIDTH				1280
#define FRAME_BUFFER_HEIGHT				800

#define VS_SLOT_CAMERA					0x00
#define VS_SLOT_WORLD_MATRIX			0x01

#define PS_SLOT_COLOR					0x00
//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01

/*정점의 색상을 무작위로(Random) 설정하기 위해 사용한다. 각 정점의 색상은 난수(Random Number)를 생성하여 지정한다.*/
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

//camera
#define FIRST_PERSON_CAMERA	0x01
#define SPACESHIP_CAMERA	0x02
#define THIRD_PERSON_CAMERA	0x03
#define ASPECT_RATIO	(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

//move
#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x10
#define DIR_DOWN	0x20

//texture
//텍스쳐와 샘플러 상태를 설정하기 위한 쉐이더의 슬롯 번호를 정의한다. 
#define PS_SLOT_TEXTURE		0x00
#define PS_SLOT_SAMPLER_STATE		0x00



//---------------------------------------------------------------------------------------------