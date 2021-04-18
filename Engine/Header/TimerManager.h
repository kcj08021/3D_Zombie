#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimerManager final:
	public CBase{
	DECLARE_SINGLETON(CTimerManager)

private:
	// ������
	explicit CTimerManager();
	// �Ҹ���
	virtual ~CTimerManager() = default;

public:
	// Ÿ�̸� �߰�
	HRESULT AddTimer(const _tchar* _szTimerKey);
	// Ÿ�̸��� �ð� ��ȭ�� ���
	_double ComputeDeltaTime(const _tchar* _szTimerKey);

private:
	// Ÿ�̸Ӹ� ������ �����̳�
	unordered_map<const _tchar*, CTimer*> m_TimerMap;
	typedef unordered_map<const _tchar*, CTimer*> TIMER_MAP;

private:
	// Ÿ�̸Ӹ� �˻��ϴ� �Լ�
	CTimer* FindTimer(const _tchar* _szTimerKey);

public:
	// Free
	virtual void Free();
};
END

#endif // !__TIMER_MANAGER_H__
