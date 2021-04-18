#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimerManager final:
	public CBase{
	DECLARE_SINGLETON(CTimerManager)

private:
	// 생성자
	explicit CTimerManager();
	// 소멸자
	virtual ~CTimerManager() = default;

public:
	// 타이머 추가
	HRESULT AddTimer(const _tchar* _szTimerKey);
	// 타이머의 시간 변화량 계산
	_double ComputeDeltaTime(const _tchar* _szTimerKey);

private:
	// 타이머를 보관할 컨테이너
	unordered_map<const _tchar*, CTimer*> m_TimerMap;
	typedef unordered_map<const _tchar*, CTimer*> TIMER_MAP;

private:
	// 타이머를 검색하는 함수
	CTimer* FindTimer(const _tchar* _szTimerKey);

public:
	// Free
	virtual void Free();
};
END

#endif // !__TIMER_MANAGER_H__
