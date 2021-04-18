#include "../Header/GraphicDevice.h"

USING(Engine)

// 싱글톤 구현
IMPLEMENT_SINGLETON(CGraphicDevice)

// 생성자
CGraphicDevice::CGraphicDevice(): m_pSDK(nullptr), m_pDevice(nullptr){
}

// 그래픽 장치 초기화 함수
HRESULT CGraphicDevice::InitGraphicDevice(HWND _hWnd, MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice){
	HRESULT hr = 0;
	//장치 초기화

	//1. IDrect3D9 객체 생성
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);          //D3D_SDK_VERSION는 매크로 상수로 다이렉트X 버전에 따라 알아서 상수 값이 잡힌다.

	if(m_pSDK == nullptr){
		return E_FAIL;
	}

	//2. 장치 조사
	//2-1. HAL을 통해 그래픽 장치 조사(HAL; Hardware Abstraction Layer, 하드웨어 추상화 계층)

	D3DCAPS9 d3dcaps;                                       //그래픽 카드에 대한 정보가 들어가는 구조체.
	ZeroMemory(&d3dcaps, sizeof(D3DCAPS9));

	//GetDeviceCaps()함수로 그래픽카드 정보를 조사하여 마지막 인자의 D3DCAPS9 구조체 변수에 결과를 넣음
	hr = m_pSDK->GetDeviceCaps(
		D3DADAPTER_DEFAULT,                                 //D3DADAPTER_DEFAULT는 장착된 그래픽 카드의 기본값                     
		D3DDEVTYPE_HAL,                                     //D3DDEVTYPE_HAL는 HAL을 이용하여 장치를 조사한다는 것
		&d3dcaps
	);

	if(FAILED(hr)){

		return E_FAIL;
	}
	//해당 함수는 결과로 HRESULT를 반환한다.(FAIL 체크를 하면 좋다.)

	//2-2. 해당 장치가 버텍스 프로세싱을 지원하는지 조사.    *버텍스 프로세싱: 버텍스 변환 + 조명 처리, 게임을 만드는데 필요한 기술들

	DWORD vp = 0;

	if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	vp |= D3DCREATE_MULTITHREADED;								//멀티스레드 이용

	//3. IDrect3Device9 객체 생성

	D3DPRESENT_PARAMETERS d3dpp;                                //현재 렌더링 환경을 담는 구조체, 개발자 자신이 값을 채워넣어야함.
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	//d3dpp 값 채우기
	d3dpp.BackBufferWidth = _uiWinCX;                              //후면 버퍼 가로크기
	d3dpp.BackBufferHeight = _uiWinCY;                             //후면 버퍼 세로크기
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;		            //후면 버퍼 픽셀 포맷(D3DFMT_A8R8G8B8: ARGB 각 8비트라는 의미의 상수)
	d3dpp.BackBufferCount = 1;                                  //후면 버퍼의 갯수(1을 준다.)

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                //멀티 샘플링 종류(NONE은 쓰지 않는다.)
	d3dpp.MultiSampleQuality = 0;                               //멀티 샘플링 품질(NONE일때 0)

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                   //화면 깜박이는 것을 어떤 방식으로 해결할 것인가 에 대한 상수값
																//D3DSWAPEFFECT_DISCARD: 스왑 체인 방식(한 프레임마다 버퍼 두개를 번갈아가며 출력, 이러한 방식을 페이지 플립핑 방식이라 함)
																//D3DSWAPEFFECT_FLIP: 
																//D3DSWAPEFFECT_COPY: API에서 사용했던 더블 버퍼링 방법과 같은 복사 방법

	d3dpp.hDeviceWindow = _hWnd;                               //장치를 사용할 창
	d3dpp.Windowed = _eMode;									//창모드 여부(MODE_WINDOWED는 enum으로 1이기 때문에 TRUE)

	d3dpp.EnableAutoDepthStencil = TRUE;                        //깊이 버퍼와 스텐실 버퍼를 자동으로 관리하는지에 대한 BOOL값
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;                //깊이 버퍼와 스텐실 버퍼에 대한 포맷값(D3DFMT_D24S8: 깊이 24비트, 스텐실 8비트)
																//깊이 버퍼:    깊이값을 표현하기 위한 버퍼(깊이는 원근을 표현하기 위한 방법, 깊이가 있어야 3D)
																//스텐실 버퍼:  후면 버퍼에 렌더링 되는 것이 막는 기법(후면 버퍼에 렌더가 될것과 안될것을 구분 하는 버퍼)

	d3dpp.Flags = 0;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //전체화면일때 사용할 주사율, 창모드일땐 0을 준다.(창모드일때는 OS가 알아서 주사율을 설정함)
																//D3DPRESENT_RATE_DEFAULT는 현재 사용중인 모니터의 주사율
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //간격 처리방법(D3DPRESENT_INTERVAL_IMMEDIATE는 간격이 발생해도 즉시 시연)
	//

	hr = m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hWnd, vp, &d3dpp, &m_pDevice);
	if(FAILED(hr)){
		return E_FAIL;
	}

	if(nullptr != _ppDevice){
		*_ppDevice = m_pDevice;
		SafeAddRef(m_pDevice);
	}

	return NOERROR;
}

// Free
void CGraphicDevice::Free(){
	//COM객체 해제, COM객체는 다 Release()함수를 가지고 있다.

	//COM객체 해제, Device객체를 먼저 해제하고, SDK객체를 해제해야 한다.
	//COM객체는 레퍼런스 카운트 기법을 이용하여 얕은 복사를 사용한다.
	//따라서 SDK객체로 만들어진 Device를 먼저 해제하고 SDK를 해제해야 한다.
	//COM객체의 Release()함수는 ULONG타입의 값을 반환한다. 이때 반환하는 것이 레퍼런스 카운터의 값.
	if(SafeRelease(m_pDevice))
		MessageBox(0, L"m_pDevice Release Failed", L"ERROR", MB_OK);
	if(SafeRelease(m_pSDK))
		MessageBox(0, L"m_pSDK Release Failed", L"ERROR", MB_OK);
}
