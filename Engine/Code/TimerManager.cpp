#include "..\Header\TimerManager.h"

IMPLEMENT_SINGLETON(CTimerManager)

// 생성자
CTimerManager::CTimerManager(){
}

// 타이머 추가
HRESULT CTimerManager::AddTimer(const _tchar * _szTimerKey){
	// 타이머 검색(기존에 있는 타이머인 경우 추가 실패)
	CTimer* pTimer = FindTimer(_szTimerKey);
	if(nullptr != pTimer)
		return E_FAIL;

	// 타이머 생성
	CTimer* pNewTimer = CTimer::Create();
	if(nullptr == pNewTimer)
		return E_FAIL;

	// 해당 타이머를 컨테이너에 보관
	m_TimerMap[_szTimerKey] = pNewTimer;

	return NOERROR;
}

// 타이머의 시간 변화량 계산
_double CTimerManager::ComputeDeltaTime(const _tchar * _szTimerKey){
	// 타이머 검색
	CTimer* pTimer = FindTimer(_szTimerKey);
	if(nullptr == pTimer)
		return 0.0;

	// 찾은 타이머의 시간 변화량 계산
	return pTimer->ComputeDeltaTime();
}

// 타이머를 검색하는 함수
CTimer * CTimerManager::FindTimer(const _tchar * _szTimerKey){
	// Find_if() 함수를 통해 타이머 검색
	TIMER_MAP::iterator find_iter = find_if(m_TimerMap.begin(), m_TimerMap.end(), IsEqual_tchar(_szTimerKey));
	if(m_TimerMap.end() == find_iter)
		return nullptr;

	return find_iter->second;
}

void CTimerManager::Free(){
	for(auto& elem : m_TimerMap)
		SafeRelease(elem.second);
	m_TimerMap.clear();
}
