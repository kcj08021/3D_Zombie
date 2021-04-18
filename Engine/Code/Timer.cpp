#include "..\Header\Timer.h"

// ������
CTimer::CTimer(){
	::ZeroMemory(&m_tFrameTime,sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tFixedTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tLastTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_tCpuTick, sizeof(LARGE_INTEGER));
}

// Ÿ�̸� �ʱ�ȭ
HRESULT CTimer::InitTimer(){
	::QueryPerformanceCounter(&m_tFrameTime);
	::QueryPerformanceCounter(&m_tFixedTime);
	::QueryPerformanceCounter(&m_tLastTime);
	::QueryPerformanceFrequency(&m_tCpuTick);

	return NOERROR;
}

// �ð� ��ȭ�� ���
_double CTimer::ComputeDeltaTime(){
	// ���� ī���� �� �о��
	::QueryPerformanceCounter(&m_tFrameTime);

	// ���� ���� �ð����� 1�� �̻� �귶�� ��� CPU ������ ����
	/* 1�ʴ� (���� ��� ī���� - ���� ��� ī����) == CPU ������ */
	if(m_tFrameTime.QuadPart - m_tFixedTime.QuadPart >= m_tCpuTick.QuadPart){
		::QueryPerformanceFrequency(&m_tCpuTick);
		m_tFixedTime = m_tFrameTime;
	}
	
	// ���� ��� ī���͸� �̿��Ͽ� �ð� ��ȭ�� ���
	/* (���� ��� ī���� - ���� ��� ī����) / CPU������ */
	_double dDeltaTime = static_cast<_double>(m_tFrameTime.QuadPart - m_tLastTime.QuadPart) / m_tCpuTick.QuadPart;

	// ���� ��� ī���͸� ������
	::QueryPerformanceCounter(&m_tLastTime);

	return dDeltaTime;
}

// ���� �Լ�(static)
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
