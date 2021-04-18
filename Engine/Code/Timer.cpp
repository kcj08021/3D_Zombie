#include "..\Header\Timer.h"

// 생성자
CTimer::CTimer(){
	::ZeroMemory(&m_tFrameTime,sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tFixedTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tLastTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tCpuTick, sizeof(LARGE_INTEGER));
}

// 타이머 초기화
HRESULT CTimer::InitTimer(){
	::QueryPerformanceCounter(&m_tFrameTime);
	::QueryPerformanceCounter(&m_tFixedTime);
	::QueryPerformanceCounter(&m_tLastTime);
	::QueryPerformanceFrequency(&m_tCpuTick);

	return NOERROR;
}

// 시간 변화량 계산
_double CTimer::ComputeDeltaTime(){
	// 현재 카운터 수 읽어옴
	::QueryPerformanceCounter(&m_tFrameTime);

	// 이전 갱신 시간에서 1초 이상 흘렀을 경우 CPU 진동수 갱신
	/* 1초는 (현재 기록 카운터 - 고정 기록 카운터) == CPU 진동수 */
	if(m_tFrameTime.QuadPart - m_tFixedTime.QuadPart >= m_tCpuTick.QuadPart){
		::QueryPerformanceFrequency(&m_tCpuTick);
		m_tFixedTime = m_tFrameTime;
	}
	
	// 이전 기록 카운터를 이용하여 시간 변화량 계산
	/* (현재 기록 카운터 - 이전 기록 카운터) / CPU진동수 */
	_double dDeltaTime = static_cast<_double>(m_tFrameTime.QuadPart - m_tLastTime.QuadPart) / m_tCpuTick.QuadPart;

	// 이전 기록 카운터를 갱신함
	::QueryPerformanceCounter(&m_tLastTime);

	return dDeltaTime;
}

// 생성 함수(static)
CTimer * CTimer::Create(){
	CTimer* pInstance = new CTimer();
	if(FAILED(pInstance->InitTimer())){
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Free
void CTimer::Free(){
}
