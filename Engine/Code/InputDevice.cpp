#include "..\Header\InputDevice.h"

// 싱글톤 구현
IMPLEMENT_SINGLETON(CInputDevice)

// 생성자
CInputDevice::CInputDevice()
	: m_pSDK(nullptr)
	, m_pKeyboard(nullptr)
	, m_pMouse(nullptr){
	::ZeroMemory(&m_byKeyboardState, sizeof(m_byKeyboardState));
	::ZeroMemory(&m_tMouseState, sizeof(DIMOUSESTATE));
}

// 입력 장치 초기화
HRESULT CInputDevice::InitInputDevice(HINSTANCE _hInst, HWND _hWnd){
	// 입력장치 COM객체 생성
	if(FAILED(::DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_pSDK), nullptr)))
		return E_FAIL;
	
	// 키보드 COM객체 생성
	if(FAILED(m_pSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;
	// 키보드의 데이터 형식 선택
	if(FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	// 키보드의 협조레벨 선택(기본 장치(키보드1과 마우스1)은 NONEXCLUSIVE와 BACKGROUND를 항상 해야함)
	if(FAILED(m_pKeyboard->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND/* | DISCL_NOWINKEY*/)))
		return E_FAIL;
	// 설정한 키보드 사용
	if(FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	// 마우스 COM객체 생성
	if(FAILED(m_pSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;
	// 마우스의 데이터 형식 선택
	if(FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	// 마우스의 협조레벨 선택(기본 장치(키보드1과 마우스1)은 NONEXCLUSIVE와 BACKGROUND를 항상 해야함)
	if(FAILED(m_pMouse->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND/* | DISCL_NOWINKEY*/)))
		return E_FAIL;
	// 설정한 마우스 사용
	if(FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return NOERROR;
}

// 매 프레임 마다 키 입력과 마우스 버튼, 움직임 상태를 저장하는 함수
HRESULT CInputDevice::UpdateInputState(){
	if(nullptr == m_pKeyboard || nullptr == m_pMouse)
		return E_FAIL;

	// 이전 프레임의 키보드 상태 저장
	::memcpy(m_byKeyboardPreState, m_byKeyboardState, KEY_INPUT_MAX);
	
	// 키보드의 입력 상태 가져옴(눌려진 키는 -값을 가지게 됨)
	if(FAILED(m_pKeyboard->GetDeviceState(KEY_INPUT_MAX, m_byKeyboardState)))
		return E_FAIL;

	// 이전 프레임의 마우스 버튼 상태 저장
	::memcpy(m_byMousePreButtonState, m_tMouseState.rgbButtons, MOUSE_BUTTON_INPUT_MAX);

	// 마우스의 입력 상태 가져옴(마우스의 움직임과 휠의 움직임, 각 키의 클릭 상태)
	if(FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_tMouseState)))
		return E_FAIL;

	return NOERROR;
}

_bool CInputDevice::IsKeyPressing(_ubyte _DIKey) const{
	if(m_byKeyboardState[_DIKey] & 0x80)
		return true;

	return false;
}

_bool CInputDevice::IsKeyDown(_ubyte _DIKey) const{
	if((m_byKeyboardState[_DIKey] & 0x80) 
		&& !(m_byKeyboardPreState[_DIKey] & 0x80))
		return true;

	return false;
}

_bool CInputDevice::IsKeyUp(_ubyte _DIKey) const{
	if(!(m_byKeyboardState[_DIKey] & 0x80) 
		&& (m_byKeyboardPreState[_DIKey] & 0x80))
		return true;

	return false;
}

_bool CInputDevice::IsMouseButtonPressing(MOUSE_BUTTON _eMouseButton) const{
	if(m_tMouseState.rgbButtons[_eMouseButton] & 0x80)
		return true;

	return false;
}

_bool CInputDevice::IsMouseButtonDown(MOUSE_BUTTON _eMouseButton) const{
	if((m_tMouseState.rgbButtons[_eMouseButton] & 0x80) 
		&& !(m_byMousePreButtonState[_eMouseButton] & 0x80))
		return true;

	return false;
}

_bool CInputDevice::IsMouseButtonUp(MOUSE_BUTTON _eMouseButton) const{
	if(!(m_tMouseState.rgbButtons[_eMouseButton] & 0x80) 
		&& (m_byMousePreButtonState[_eMouseButton] & 0x80))
		return true;

	return false;
}

// Free
void CInputDevice::Free(){
	SafeRelease(m_pMouse);
	SafeRelease(m_pKeyboard);
	SafeRelease(m_pSDK);
}
