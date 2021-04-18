#ifndef __TIMER_H__
#define __TIMER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer final:
	public CBase{
private:
	// 생성자
	explicit CTimer();
	// 소멸자
	virtual ~CTimer() = default;

private:
	// 타이머 초기화
	HRESULT InitTimer();

public:
	// 시간 변화량 계산
	_double ComputeDeltaTime();

private:
	// 현재 기록 카운터
	LARGE_INTEGER m_tFrameTime;
	// 고정 기록 카운터
	LARGE_INTEGER m_tFixedTime;
	// 이전 기록 카운터
	LARGE_INTEGER m_tLastTime;
	// CPU 진동수
	LARGE_INTEGER m_tCpuTick;

public:
	// 생성 함수(static)
	static CTimer* Create();
	// Free
	virtual void Free();
};

END



#endif // !__TIMER_H__
