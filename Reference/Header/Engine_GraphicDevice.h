#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CGraphicDevice final: public CBase{
	// �̱��� ó��
	DECLARE_SINGLETON(CGraphicDevice)

	// ��� ������ ����
public:
	enum MODE{ MODE_FULLSCREEN, MODE_WINDOWED};

	// �����ڿ� �Ҹ���
private:
	explicit CGraphicDevice();
	virtual ~CGraphicDevice() = default;

	// Get �Լ�
public:
	LPDIRECT3DDEVICE9 GetGraphicDevice(){ return m_pDevice; }
	
	// Set �Լ�
public:


	// Public ��� �Լ�
public:
	HRESULT InitGraphicDevice(HWND _hWnd, MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice);       //��ġ �ʱ�ȭ �Լ�

	//private �������
private:
	LPDIRECT3D9			m_pSDK		= nullptr;      //IDrect3D9�� ������, ��ġ�� ���� �� ���� �ϰ�, IDrect3Device9�� �����ϴ� ����.
	LPDIRECT3DDEVICE9	m_pDevice	= nullptr;		//IDrect3Device9�� ������, ��ġ�� ���ؼ� �׷��� �������� �����ϴ� ����.
													//���⼭ I�� Interface�� ����
													//�̷��� I�� �����ϴ� �ڷ����� COM��ü ��� �θ���.(COM: Component Object Model)

	// CBase��(��) ���� ��ӵ�
public:
	virtual void Free() override;
};

END

#endif // !__DEVICE_MANAGER_H__
