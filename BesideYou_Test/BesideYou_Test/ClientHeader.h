#pragma once

#include "targetver.h"

// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <d3d11.h>
#include <d3dx11.h>
#include <mmsystem.h>
#include <math.h>
#include <d3dcompiler.h>  	//���̴� ������ �Լ��� ����ϱ� ���� ��� ����
#include <D3DX10Math.h>	//Direct3D ���� �Լ��� ����ϱ� ���� ��� ����
#include <D3D9Types.h>

//���̷�Ʈ2d
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <DxErr.h>

//
#include "BesideYou_Test.h"
#include "Timer.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "Scene.h"
#include "GameFramework.h"

//---------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#define FRAME_BUFFER_WIDTH				1280
#define FRAME_BUFFER_HEIGHT				800

#define VS_SLOT_CAMERA					0x00
#define VS_SLOT_WORLD_MATRIX			0x01

#define PS_SLOT_COLOR					0x00

/*������ ������ ��������(Random) �����ϱ� ���� ����Ѵ�. �� ������ ������ ����(Random Number)�� �����Ͽ� �����Ѵ�.*/
#define RANDOM_COLOR D3DXCOLOR((rand() * 0xFFFFFF) / RAND_MAX)

#define FIRST_PERSON_CAMERA	0x01
#define SPACESHIP_CAMERA	0x02
#define THIRD_PERSON_CAMERA	0x03
#define ASPECT_RATIO	(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

#define DIR_FORWARD	0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT	0x04
#define DIR_RIGHT	0x08
#define DIR_UP		0x10
#define DIR_DOWN	0x20
//---------------------------------------------------------------------------------------------