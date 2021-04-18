#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CGraphicDevice final: public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CGraphicDevice)

public:
	// 실행 모드 열거체
	enum MODE{ MODE_FULLSCREEN, MODE_WINDOWED};

private:
	// 생성자
	explicit CGraphicDevice();
	// 소멸자
	virtual ~CGraphicDevice() = default;

	// Get
public:
	// 그래픽 장치 변수 얻어옴
	LPDIRECT3DDEVICE9 GetGraphicDevice(){ return m_pDevice; }

public:
	// 그래픽 장치 초기화 함수
	HRESULT InitGraphicDevice(HWND _hWnd, MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice);       //장치 초기화 함수

private:
	LPDIRECT3D9			m_pSDK		= nullptr;      //IDrect3D9의 포인터, 장치를 조사 및 검증 하고, IDrect3Device9를 생성하는 역할.
	LPDIRECT3DDEVICE9	m_pDevice	= nullptr;		//IDrect3Device9의 포인터, 장치를 통해서 그래픽 렌더링을 수행하는 역할.
													//여기서 I는 Interface의 약자
													//이렇게 I로 시작하는 자료형을 COM객체 라고 부른다.(COM: Component Object Model)

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__DEVICE_MANAGER_H__
