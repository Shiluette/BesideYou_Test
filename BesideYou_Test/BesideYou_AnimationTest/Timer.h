#pragma once
// 50회의 프레임 처리시간을 누적하여 평균한다.
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
	bool m_bHardwareHasPerformanceCounter;//컴퓨터가 Performance Counter를 가지고 있는 가
	float m_fTimeScale;//Scale Counter의 양
	float m_fTimeElapsed;//마지막 프레임 이후 지나간 시간
	__int64 m_nCurrentTime;//현재의 시간
	__int64 m_nLastTime;//마지막 프레임의 시간
	__int64 m_PerformanceFrequency;//컴퓨터의 Performance Frequency

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_FramePerSecond;
	float m_fFPSTimeElapsed;
};

