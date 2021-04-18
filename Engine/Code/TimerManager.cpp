#include "..\Header\TimerManager.h"

IMPLEMENT_SINGLETON(CTimerManager)

// ������
CTimerManager::CTimerManager(){
}

// Ÿ�̸� �߰�
HRESULT CTimerManager::AddTimer(const _tchar * _szTimerKey){
	// Ÿ�̸� �˻�(������ �ִ� Ÿ�̸��� ��� �߰� ����)
	CTimer* pTimer = FindTimer(_szTimerKey);
	if(nullptr != pTimer)
		return E_FAIL;

	// Ÿ�̸� ����
	CTimer* pNewTimer = CTimer::Create();
	if(nullptr == pNewTimer)
		return E_FAIL;

	// �ش� Ÿ�̸Ӹ� �����̳ʿ� ����
	m_TimerMap[_szTimerKey] = pNewTimer;

	return NOERROR;
}

// Ÿ�̸��� �ð� ��ȭ�� ���
_double CTimerManager::ComputeDeltaTime(const _tchar * _szTimerKey){
	// Ÿ�̸� �˻�
	CTimer* pTimer = FindTimer(_szTimerKey);
	if(nullptr == pTimer)
		return 0.0;

	// ã�� Ÿ�̸��� �ð� ��ȭ�� ���
	return pTimer->ComputeDeltaTime();
}

// Ÿ�̸Ӹ� �˻��ϴ� �Լ�
CTimer * CTimerManager::FindTimer(const _tchar * _szTimerKey){
	// Find_if() �Լ��� ���� Ÿ�̸� �˻�
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
