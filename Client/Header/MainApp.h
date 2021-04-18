#ifndef __MAINAPP_H__
#define __MAINAPP_H__
// Client 헤더
#include "Client_Headers.h"

// Engine 헤더
#include "Base.h"
#include "Management.h"

BEGIN(Client)
class CMainApp final: public CBase{
private:
	// 생성자
	explicit CMainApp();
	// 소멸자
	virtual ~CMainApp() = default;

private:
	// Initialize
	HRESULT InitMainApp();
public:
	// Update
	HRESULT UpdateMainApp(_double _dDeltaTime);
	// Render
	HRESULT RenderMainApp(_double _dDeltaTime);

protected:

private:
	// 매니지먼트 변수, 엔진과 클라이언트 사이의 연결
	CManagement* m_pManagement = nullptr;
	// 그래픽 디바이스 변수, 말 그대로 디바이스 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 렌더러 컴포넌트 변수
	CRenderer* m_pRendererCom = nullptr;

	// 입력 상태 업데이트 실패 확인 변수(실패시 한번만 메세지 박스 나오도록)
	_bool m_bIsInputStateUpdateFailure = false;

private:
	// 초기에 멤버 변수들을 세팅하는 함수
	HRESULT SettingDefault(CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY);
	// 씬을 세팅하는 함수
	HRESULT SettingScene(SCENE_TYPE _eSceneType);

	/* 이 부분에 있는 함수는 테스트용 함수들 */
	// 렌더러 컴포넌트를 등록하는 함수
	HRESULT AddRenderComponent();
	// 셰이더 컴포넌트를 등록하는 함수
	HRESULT AddShaderComponent();
	// 사각형을 그리기 위한 버텍스 버퍼를 만드는 컴포넌트를 등록하는 함수
	HRESULT AddVIBufferRectComponent();
	// 지형을 그리기 위한 버텍스 버퍼를 만드는 컴포넌트를 등록하는 함수
	HRESULT AddVIBufferTerrainComponent();
	// 텍스쳐를 로드하여 컴포넌트로 등록하는 함수
	HRESULT AddTextureComponent();
	// CTransform 컴 포넌트를 등록하는 함수
	HRESULT AddTransformComponent();
	// 피킹을 위한 컴포넌트를 등록하는 함수
	HRESULT AddPickingComponent();
	// 콜라이더 컴포넌트를 등록하는 함수
	HRESULT AddColliderComponent();
	// 스카이박스 오브젝트의 프로토타입을 등록하는 함수
	HRESULT AddSkyBoxObject();
	// 절두체 컴포넌트의 프로토타입을 등록하는 함수
	HRESULT AddFrustumComponent();
public:
	// 생성 함수(static)
	static CMainApp* Create();

public:
	// Free
	virtual void Free() override;
};
END
#endif // !__MAINAPP_H__