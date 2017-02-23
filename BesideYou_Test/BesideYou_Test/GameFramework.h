#pragma once

//#include "Timer.h"
//#include "Player.h"
//#include "Scene.h"

class CGameFramework
{
private:
	//--------------------------------------------------------------------
	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	CGameTimer m_GameTimer;
	//������ ������ ���(Scene)�� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����.
	CScene *m_pScene;
	//������ ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.
	_TCHAR m_pszBuffer[100];
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	CPlayer *m_pPlayer;
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	//����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device *m_pd3dDevice;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain *m_pDXGISwapChain;
	//����̽� ���ؽ�Ʈ�� ���� �������̴�. �ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	//���� Ÿ�� �� �������̽��� ���� �������̴�. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;

	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	ID3D11Buffer *m_pd3dcbColor;
	//---------------------------------------------------------------------

	//2.23
	CCamera * m_pCamera;

	//���̷�Ʈ2d
	ID2D1Factory1 *m_pd2dFactory{ nullptr };
	ID2D1Device *m_pd2dDevice{ nullptr };
	ID2D1DeviceContext *m_pd2dContext{ nullptr };
	IDWriteFactory *m_pdwFactory{ nullptr };
	IWICImagingFactory *m_pwicFactory{ nullptr };

	//2dObject
	IDWriteTextFormat* m_dwExplainFormat{ nullptr };
	IDWriteTextFormat *m_dwMyChattingFormat{ nullptr };
	ID2D1RectangleGeometry *m_pd2drcBox{ nullptr };
	ID2D1SolidColorBrush *m_pd2dsbrBeige{ nullptr };
	ID2D1Bitmap1** m_ppd2dBitmap;
	ID2D1SolidColorBrush * m_pd2dsbrGreenColor{ nullptr };
	ID2D1SolidColorBrush * m_pd2dsbrRedColor{ nullptr };
	int m_nBitmaps;

	//��Ƽ���ø�
	UINT                     m_n4xMSAAQualities;
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetDepthStencilView();

	bool CreateDirect3DDisplay();

	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects();
	void ReleaseObjects();

	//���̷�Ʈ2d
	void Build2DObjects();
	void Render2D();
	void Release2DObjects();
	bool LoadImageFromFile(_TCHAR *pszstrFileName, ID2D1Bitmap1 **ppd2dBitmap, D2D_RECT_U *pd2drcImage, UINT nWidth, UINT nHeight, WICBitmapTransformOptions nFlipRotation);

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�.
	POINT	m_ptOldCursorPos;

	CPlayerShader *m_pPlayerShader;
};