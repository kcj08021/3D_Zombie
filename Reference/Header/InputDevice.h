#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__

#include "Base.h"

BEGIN(Engine)

class CInputDevice:
	public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CInputDevice)
public:
	// ���콺 ��ư ����ü
	enum MOUSE_BUTTON{ MOUSE_LBUTTON, MOUSE_RBUTTON, MOUSE_MBUTTON };
	// ���콺 ������ ����ü
	enum MOUSE_MOVE{ MOUSE_X, MOUSE_Y, MOUSE_WHEEL};
private:
	// Ű �Է� �ִ밪(���)
	enum{ KEY_INPUT_MAX = 256};
	// ���콺 ��ư �Է� �ִ밪(���)
	enum{ MOUSE_BUTTON_INPUT_MAX = 4 };

private:
	// ������
	explicit CInputDevice();
	// �Ҹ���
	virtual ~CInputDevice() = default;

public:
	// �Է� ��ġ �ʱ�ȭ
	HRESULT InitInputDevice(HINSTANCE _hInst, HWND _hWnd);
	// �� ������ ���� Ű �Է°� ���콺 ��ư, ������ ���¸� �����ϴ� �Լ�
	HRESULT UpdateInputState();

public:
	// �Է��� Ű�� ���ȴ��� �˻�
	_bool IsKeyPressing(_ubyte _DIKey) const;
	// Ű������ ��ư�� ���� ���ĸ� ��
	_bool IsKeyDown(_ubyte _DIKey) const;
	// Ű������ ��ư�� ���� ���°� Ǯ�������� ��
	_bool IsKeyUp(_ubyte _DIKey) const;

	// ���콺 ��ư�� ���ȴ��� �˻�
	_bool IsMouseButtonPressing(MOUSE_BUTTON _eMouseButton) const;
	// ���콺�� ��ư�� ���� ���ĸ� ��
	_bool IsMouseButtonDown(MOUSE_BUTTON _eMouseButton) const;
	// ���콺�� ��ư�� ���� ���°� Ǯ�������� ��
	_bool IsMouseButtonUp(MOUSE_BUTTON _eMouseButton) const;

	// ���콺�� ������ ������ ������
	_long GetMouseMovement(MOUSE_MOVE _eMouseMove) const{
		return reinterpret_cast<const _long*>(&m_tMouseState)[_eMouseMove];
	}


private:
	// �Է� COM ��ü
	LPDIRECTINPUT8 m_pSDK = nullptr;
	// Ű���� COM ��ü
	LPDIRECTINPUTDEVICE8 m_pKeyboard = nullptr;
	// ���콺 COM ��ü
	LPDIRECTINPUTDEVICE8 m_pMouse = nullptr;

	// Ű���� ���� ���� �迭
	_byte m_byKeyboardState[KEY_INPUT_MAX];
	// Ű���� ���� ���� �迭
	_byte m_byKeyboardPreState[KEY_INPUT_MAX];
	// ���콺 ���� ���� ����ü
	DIMOUSESTATE m_tMouseState;
	// ���콺 ���� ��ư ���� ����ü
	_byte m_byMousePreButtonState[MOUSE_BUTTON_INPUT_MAX];

public:
	// Free
	virtual void Free();
};

END

#endif // !__INPUT_DEVICE_H__

