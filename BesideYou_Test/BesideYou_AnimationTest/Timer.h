#pragma once
// 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.
const ULONG MAX_SAMPLE_COUNT = 50; 

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();

private:
	bool m_bHardwareHasPerformanceCounter;//��ǻ�Ͱ� Performance Counter�� ������ �ִ� ��
	float m_fTimeScale;//Scale Counter�� ��
	float m_fTimeElapsed;//������ ������ ���� ������ �ð�
	__int64 m_nCurrentTime;//������ �ð�
	__int64 m_nLastTime;//������ �������� �ð�
	__int64 m_PerformanceFrequency;//��ǻ���� Performance Frequency

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_FramePerSecond;
	float m_fFPSTimeElapsed;
};

