#include "../Header/GraphicDevice.h"

USING(Engine)

// �̱��� ����
IMPLEMENT_SINGLETON(CGraphicDevice)

// ������
CGraphicDevice::CGraphicDevice(): m_pSDK(nullptr), m_pDevice(nullptr){
}

// �׷��� ��ġ �ʱ�ȭ �Լ�
HRESULT CGraphicDevice::InitGraphicDevice(HWND _hWnd, MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice){
	HRESULT hr = 0;
	//��ġ �ʱ�ȭ

	//1. IDrect3D9 ��ü ����
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);          //D3D_SDK_VERSION�� ��ũ�� ����� ���̷�ƮX ������ ���� �˾Ƽ� ��� ���� ������.

	if(m_pSDK == nullptr){
		return E_FAIL;
	}

	//2. ��ġ ����
	//2-1. HAL�� ���� �׷��� ��ġ ����(HAL; Hardware Abstraction Layer, �ϵ���� �߻�ȭ ����)

	D3DCAPS9 d3dcaps;                                       //�׷��� ī�忡 ���� ������ ���� ����ü.
	ZeroMemory(&d3dcaps, sizeof(D3DCAPS9));

	//GetDeviceCaps()�Լ��� �׷���ī�� ������ �����Ͽ� ������ ������ D3DCAPS9 ����ü ������ ����� ����
	hr = m_pSDK->GetDeviceCaps(
		D3DADAPTER_DEFAULT,                                 //D3DADAPTER_DEFAULT�� ������ �׷��� ī���� �⺻��                     
		D3DDEVTYPE_HAL,                                     //D3DDEVTYPE_HAL�� HAL�� �̿��Ͽ� ��ġ�� �����Ѵٴ� ��
		&d3dcaps
	);

	if(FAILED(hr)){

		return E_FAIL;
	}
	//�ش� �Լ��� ����� HRESULT�� ��ȯ�Ѵ�.(FAIL üũ�� �ϸ� ����.)

	//2-2. �ش� ��ġ�� ���ؽ� ���μ����� �����ϴ��� ����.    *���ؽ� ���μ���: ���ؽ� ��ȯ + ���� ó��, ������ ����µ� �ʿ��� �����

	DWORD vp = 0;

	if(d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	vp |= D3DCREATE_MULTITHREADED;								//��Ƽ������ �̿�

	//3. IDrect3Device9 ��ü ����

	D3DPRESENT_PARAMETERS d3dpp;                                //���� ������ ȯ���� ��� ����ü, ������ �ڽ��� ���� ä���־����.
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	//d3dpp �� ä���
	d3dpp.BackBufferWidth = _uiWinCX;                              //�ĸ� ���� ����ũ��
	d3dpp.BackBufferHeight = _uiWinCY;                             //�ĸ� ���� ����ũ��
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;		            //�ĸ� ���� �ȼ� ����(D3DFMT_A8R8G8B8: ARGB �� 8��Ʈ��� �ǹ��� ���)
	d3dpp.BackBufferCount = 1;                                  //�ĸ� ������ ����(1�� �ش�.)

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;                //��Ƽ ���ø� ����(NONE�� ���� �ʴ´�.)
	d3dpp.MultiSampleQuality = 0;                               //��Ƽ ���ø� ǰ��(NONE�϶� 0)

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;                   //ȭ�� �����̴� ���� � ������� �ذ��� ���ΰ� �� ���� �����
																//D3DSWAPEFFECT_DISCARD: ���� ü�� ���(�� �����Ӹ��� ���� �ΰ��� �����ư��� ���, �̷��� ����� ������ �ø��� ����̶� ��)
																//D3DSWAPEFFECT_FLIP: 
																//D3DSWAPEFFECT_COPY: API���� ����ߴ� ���� ���۸� ����� ���� ���� ���

	d3dpp.hDeviceWindow = _hWnd;                               //��ġ�� ����� â
	d3dpp.Windowed = _eMode;									//â��� ����(MODE_WINDOWED�� enum���� 1�̱� ������ TRUE)

	d3dpp.EnableAutoDepthStencil = TRUE;                        //���� ���ۿ� ���ٽ� ���۸� �ڵ����� �����ϴ����� ���� BOOL��
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;                //���� ���ۿ� ���ٽ� ���ۿ� ���� ���˰�(D3DFMT_D24S8: ���� 24��Ʈ, ���ٽ� 8��Ʈ)
																//���� ����:    ���̰��� ǥ���ϱ� ���� ����(���̴� ������ ǥ���ϱ� ���� ���, ���̰� �־�� 3D)
																//���ٽ� ����:  �ĸ� ���ۿ� ������ �Ǵ� ���� ���� ���(�ĸ� ���ۿ� ������ �ɰͰ� �ȵɰ��� ���� �ϴ� ����)

	d3dpp.Flags = 0;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //��üȭ���϶� ����� �ֻ���, â����϶� 0�� �ش�.(â����϶��� OS�� �˾Ƽ� �ֻ����� ������)
																//D3DPRESENT_RATE_DEFAULT�� ���� ������� ������� �ֻ���
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //���� ó�����(D3DPRESENT_INTERVAL_IMMEDIATE�� ������ �߻��ص� ��� �ÿ�)
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
	//COM��ü ����, COM��ü�� �� Release()�Լ��� ������ �ִ�.

	//COM��ü ����, Device��ü�� ���� �����ϰ�, SDK��ü�� �����ؾ� �Ѵ�.
	//COM��ü�� ���۷��� ī��Ʈ ����� �̿��Ͽ� ���� ���縦 ����Ѵ�.
	//���� SDK��ü�� ������� Device�� ���� �����ϰ� SDK�� �����ؾ� �Ѵ�.
	//COM��ü�� Release()�Լ��� ULONGŸ���� ���� ��ȯ�Ѵ�. �̶� ��ȯ�ϴ� ���� ���۷��� ī������ ��.
	if(SafeRelease(m_pDevice))
		MessageBox(0, L"m_pDevice Release Failed", L"ERROR", MB_OK);
	if(SafeRelease(m_pSDK))
		MessageBox(0, L"m_pSDK Release Failed", L"ERROR", MB_OK);
}
