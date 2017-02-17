// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define FRAME_BUFFER_WIDTH	640
#define FRAME_BUFFER_HEIGHT	480

#define PS_SLOT_COLOR	0x00

#define VS_SLOT_CAMERA				0x00
#define VS_SLOT_WORLD_MATRIX			0x01

//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x00
#define PS_SLOT_MATERIAL		0x01

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

//텍스쳐와 샘플러 상태를 설정하기 위한 쉐이더의 슬롯 번호를 정의한다. 
#define PS_SLOT_TEXTURE		0x00
#define PS_SLOT_SAMPLER_STATE		0x00



// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <Mmsystem.h>
#include <math.h>
#include <d3dcompiler.h> //쉐이더 컴파일 함수 사용
#include <D3DX10math.h> //direct3d 수학 함수를 사용하기
#include <D3D9Types.h>

//1
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
const int Bone_count = 14;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
