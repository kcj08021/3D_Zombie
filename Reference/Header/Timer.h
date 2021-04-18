#ifndef __TIMER_H__
#define __TIMER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer final:
	public CBase{
private:
	// ������
	explicit CTimer();
	// �Ҹ���
	virtual ~CTimer() = default;

private:
	// Ÿ�̸� �ʱ�ȭ
	HRESULT InitTimer();

public:
	// �ð� ��ȭ�� ���
	_double ComputeDeltaTime();

private:
	// ���� ��� ī����
	LARGE_INTEGER m_tFrameTime;
	// ���� ��� ī����
	LARGE_INTEGER m_tFixedTime;
	// ���� ��� ī����
	LARGE_INTEGER m_tLastTime;
	// CPU ������
	LARGE_INTEGER m_tCpuTick;

public:
	// ���� �Լ�(static)
	static CTimer* Create();
	// Free
	virtual void Free();
};

END



#endif // !__TIMER_H__
