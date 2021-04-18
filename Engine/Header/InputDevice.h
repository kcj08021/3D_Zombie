#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__

#include "Base.h"

BEGIN(Engine)

class CInputDevice:
	public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CInputDevice)
public:
	// 마우스 버튼 열거체
	enum MOUSE_BUTTON{ MOUSE_LBUTTON, MOUSE_RBUTTON, MOUSE_MBUTTON };
	// 마우스 움직임 열거체
	enum MOUSE_MOVE{ MOUSE_X, MOUSE_Y, MOUSE_WHEEL};
private:
	// 키 입력 최대값(상수)
	enum{ KEY_INPUT_MAX = 256};
	// 마우스 버튼 입력 최대값(상수)
	enum{ MOUSE_BUTTON_INPUT_MAX = 4 };

private:
	// 생성자
	explicit CInputDevice();
	// 소멸자
	virtual ~CInputDevice() = default;

public:
	// 입력 장치 초기화
	HRESULT InitInputDevice(HINSTANCE _hInst, HWND _hWnd);
	// 매 프레임 마다 키 입력과 마우스 버튼, 움직임 상태를 저장하는 함수
	HRESULT UpdateInputState();

public:
	// 입력한 키가 눌렸는지 검사
	_bool IsKeyPressing(_ubyte _DIKey) const;
	// 키보드의 버튼이 눌린 직후만 참
	_bool IsKeyDown(_ubyte _DIKey) const;
	// 키보드의 버튼이 눌린 상태가 풀렸을때만 참
	_bool IsKeyUp(_ubyte _DIKey) const;

	// 마우스 버튼이 눌렸는지 검사
	_bool IsMouseButtonPressing(MOUSE_BUTTON _eMouseButton) const;
	// 마우스의 버튼이 눌린 직후만 참
	_bool IsMouseButtonDown(MOUSE_BUTTON _eMouseButton) const;
	// 마우스의 버튼이 눌린 상태가 풀렸을때만 참
	_bool IsMouseButtonUp(MOUSE_BUTTON _eMouseButton) const;

	// 마우스의 움직임 정도를 가져옴
	_long GetMouseMovement(MOUSE_MOVE _eMouseMove) const{
		return reinterpret_cast<const _long*>(&m_tMouseState)[_eMouseMove];
	}


private:
	// 입력 COM 객체
	LPDIRECTINPUT8 m_pSDK = nullptr;
	// 키보드 COM 객체
	LPDIRECTINPUTDEVICE8 m_pKeyboard = nullptr;
	// 마우스 COM 객체
	LPDIRECTINPUTDEVICE8 m_pMouse = nullptr;

	// 키보드 현재 상태 배열
	_byte m_byKeyboardState[KEY_INPUT_MAX];
	// 키보드 이전 상태 배열
	_byte m_byKeyboardPreState[KEY_INPUT_MAX];
	// 마우스 현재 상태 구조체
	DIMOUSESTATE m_tMouseState;
	// 마우스 이전 버튼 상태 구조체
	_byte m_byMousePreButtonState[MOUSE_BUTTON_INPUT_MAX];

public:
	// Free
	virtual void Free();
};

END

#endif // !__INPUT_DEVICE_H__

