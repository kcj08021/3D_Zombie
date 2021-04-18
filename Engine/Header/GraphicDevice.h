#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CGraphicDevice final: public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CGraphicDevice)

public:
	// ���� ��� ����ü
	enum MODE{ MODE_FULLSCREEN, MODE_WINDOWED};

private:
	// ������
	explicit CGraphicDevice();
	// �Ҹ���
	virtual ~CGraphicDevice() = default;

	// Get
public:
	// �׷��� ��ġ ���� ����
	LPDIRECT3DDEVICE9 GetGraphicDevice(){ return m_pDevice; }

public:
	// �׷��� ��ġ �ʱ�ȭ �Լ�
	HRESULT InitGraphicDevice(HWND _hWnd, MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice);       //��ġ �ʱ�ȭ �Լ�

private:
	LPDIRECT3D9			m_pSDK		= nullptr;      //IDrect3D9�� ������, ��ġ�� ���� �� ���� �ϰ�, IDrect3Device9�� �����ϴ� ����.
	LPDIRECT3DDEVICE9	m_pDevice	= nullptr;		//IDrect3Device9�� ������, ��ġ�� ���ؼ� �׷��� �������� �����ϴ� ����.
													//���⼭ I�� Interface�� ����
													//�̷��� I�� �����ϴ� �ڷ����� COM��ü ��� �θ���.(COM: Component Object Model)

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__DEVICE_MANAGER_H__
