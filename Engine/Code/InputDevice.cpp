#include "..\Header\InputDevice.h"

// �̱��� ����
IMPLEMENT_SINGLETON(CInputDevice)

// ������
CInputDevice::CInputDevice()
	: m_pSDK(nullptr)
	, m_pKeyboard(nullptr)
	, m_pMouse(nullptr){
	::ZeroMemory(&m_byKeyboardState, sizeof(m_byKeyboardState));
	::ZeroMemory(&m_tMouseState, sizeof(DIMOUSESTATE));
}

// �Է� ��ġ �ʱ�ȭ
HRESULT CInputDevice::InitInputDevice(HINSTANCE _hInst, HWND _hWnd){
	// �Է���ġ COM��ü ����
	if(FAILED(::DirectInput8Create(_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_pSDK), nullptr)))
		return E_FAIL;
	
	// Ű���� COM��ü ����
	if(FAILED(m_pSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;
	// Ű������ ������ ���� ����
	if(FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	// Ű������ �������� ����(�⺻ ��ġ(Ű����1�� ���콺1)�� NONEXCLUSIVE�� BACKGROUND�� �׻� �ؾ���)
	if(FAILED(m_pKeyboard->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND/* | DISCL_NOWINKEY*/)))
		return E_FAIL;
	// ������ Ű���� ���
	if(FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	// ���콺 COM��ü ����
	if(FAILED(m_pSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;
	// ���콺�� ������ ���� ����
	if(FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	// ���콺�� �������� ����(�⺻ ��ġ(Ű����1�� ���콺1)�� NONEXCLUSIVE�� BACKGROUND�� �׻� �ؾ���)
	if(FAILED(m_pMouse->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND/* | DISCL_NOWINKEY*/)))
		return E_FAIL;
	// ������ ���콺 ���
	if(FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return NOERROR;
}

// �� ������ ���� Ű �Է°� ���콺 ��ư, ������ ���¸� �����ϴ� �Լ�
HRESULT CInputDevice::UpdateInputState(){
	if(nullptr == m_pKeyboard || nullptr == m_pMouse)
		return E_FAIL;

	// ���� �������� Ű���� ���� ����
	::memcpy(m_byKeyboardPreState, m_byKeyboardState, KEY_INPUT_MAX);
	
	// Ű������ �Է� ���� ������(������ Ű�� -���� ������ ��)
	if(FAILED(m_pKeyboard->GetDeviceState(KEY_INPUT_MAX, m_byKeyboardState)))
		return E_FAIL;

	// ���� �������� ���콺 ��ư ���� ����
	::memcpy(m_byMousePreButtonState, m_tMouseState.rgbButtons, MOUSE_BUTTON_INPUT_MAX);

	// ���콺�� �Է� ���� ������(���콺�� �����Ӱ� ���� ������, �� Ű�� Ŭ�� ����)
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
