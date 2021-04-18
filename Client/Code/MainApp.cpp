// Client 헤더
#include "stdafx.h"
#include "..\Header\MainApp.h"
#include "..\Header\TitleScene.h"
#include "..\Header\StageScene.h"
#include "..\Header\LoadingScene.h"
#include "..\Header\SkyBox.h"
#include "..\Header\UnitManager.h"

USING(Client)

// 생성자
CMainApp::CMainApp()
	: m_pManagement(nullptr)
	, m_pGraphicDevice(nullptr)
	, m_pRendererCom(nullptr)
	, m_bIsInputStateUpdateFailure(false){
}

// Initialize
HRESULT CMainApp::InitMainApp(){
	// 디폴트로 세팅
	if(FAILED(SettingDefault(CGraphicDevice::MODE_WINDOWED, WINCX, WINCY)))
		return E_FAIL;
	// 렌더러 컴포넌트 등록
	if(FAILED(AddRenderComponent()))
		return E_FAIL;
	// 셰이더 컴포넌트 등록
	if(FAILED(AddShaderComponent()))
		return E_FAIL;
	// 사각형 버텍스 버퍼 컴포넌트 등록
	if(FAILED(AddVIBufferRectComponent()))
		return E_FAIL;
	// 텍스쳐 컴포넌트 등록
	if(FAILED(AddTextureComponent()))
		return E_FAIL;
	// Transform 컴포넌트 등록
	if(FAILED(AddTransformComponent()))
		return E_FAIL;
	// 피킹 관련 컴포넌트 등록
	if(FAILED(AddPickingComponent()))
		return E_FAIL;
	// 콜라이더 컴포넌트 등록
	if(FAILED(AddColliderComponent()))
		return E_FAIL;
	if(FAILED(AddSkyBoxObject()))
		return E_FAIL;

	if(FAILED(AddFrustumComponent()))
		return E_FAIL;

	// 씬 세팅
	if(FAILED(SettingScene(SCENE_TYPE::TITLE_SCENE)))
		return E_FAIL;

	return NOERROR;
}

// Update
HRESULT CMainApp::UpdateMainApp(_double _dDeltaTime){
	if(nullptr == m_pManagement)
		return E_FAIL;

	// 입력 상태 업데이트 
	if(FAILED(m_pManagement->UpdateInputState()))
		if(!m_bIsInputStateUpdateFailure){
			MSG_BOX(L"CInputDevice Update Failure");
			m_bIsInputStateUpdateFailure = true;
		}

	if(m_pManagement->IsKeyDown(DIK_F2))
		m_pRendererCom->SetRenderDebugBuffer(!m_pRendererCom->GetRenderDebugBuffer());

	// 매니지먼트에서 관리되고 있는 현재 씬을 업데이트
	return m_pManagement->UpdateCurrentScene(_dDeltaTime);
}

// Render
HRESULT CMainApp::RenderMainApp(_double _dDeltaTime){
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;

	if(nullptr == m_pManagement)
		return E_FAIL;

	// 그래픽 디바이스의 후면버퍼 비우기
	m_pGraphicDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	// 후면버퍼에 그리기 시작
	m_pGraphicDevice->BeginScene();

	// 렌더러 컴포넌트에서 렌더 시작
	m_pRendererCom->DoRender(_dDeltaTime);
	// 매니지먼트에서 관리되고 있는 현재 씬을 렌더
	m_pManagement->RenderCurrentScene();

	// 후면 버퍼에 그리기 끝
	m_pGraphicDevice->EndScene();
	// 현재 그려진 후면 버퍼를 전면 버퍼로 교체 및 시연
	m_pGraphicDevice->Present(nullptr, nullptr, 0, nullptr);

	return NOERROR;
}

// 초기에 기본적인 멤버 변수들을 세팅하는 함수
HRESULT CMainApp::SettingDefault(CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY){
	// 매니지먼트 객체 준비
	if(nullptr == m_pManagement){
		m_pManagement = CManagement::GetInstance();
		if(nullptr == m_pManagement)
			return E_FAIL;

		SafeAddRef(m_pManagement);
	}

	// 엔진에서 사용할 객체들을 준비
	if(FAILED(m_pManagement->InitEngine(SCENE_END)))
		return E_FAIL;

	// 그래픽 장치 준비
	if(FAILED(m_pManagement->InitGraphicDevice(g_hWnd, _eMode, _uiWinCX, _uiWinCY, &m_pGraphicDevice)))
		return E_FAIL;

	// 입력 장치 준비
	if(FAILED(m_pManagement->InitInputDevice(g_hInst, g_hWnd)))
		return E_FAIL;

	// 사운드 준비
	if(FAILED(m_pManagement->InitSound()))
		return E_FAIL;

	return NOERROR;
}

// 씬을 세팅하는 함수
HRESULT CMainApp::SettingScene(SCENE_TYPE _eSceneType){
	CScene* pScene = nullptr;

	// 매개 변수로 받아온 씬 타입에 따라 씬 결정
	switch(_eSceneType){
	case TITLE_SCENE:
		pScene = CTitleScene::Create(m_pGraphicDevice);
		break;
	case STAGE_SCENE:
		pScene = CStageScene::Create(m_pGraphicDevice);
		break;
	case LOADING_SCENE:
		pScene = CLoadingScene::Create(m_pGraphicDevice);
		break;
	default:
		break;
	}

	if(nullptr == pScene)
		return E_FAIL;

	// 설정된 씬으로 교체
	if(FAILED(m_pManagement->SetCurrentScene(pScene)))
		return E_FAIL;

	pScene->Release();

	return NOERROR;
}

// 렌더러 컴포넌트를 등록하는 함수
HRESULT CMainApp::AddRenderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE, m_pRendererCom = CRenderer::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	SafeAddRef(m_pRendererCom);

	return NOERROR;
}

// 셰이더 컴포넌트를 등록하는 함수
HRESULT CMainApp::AddShaderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"DefaultShaderCom_Proto", SCENE_TYPE::STATIC_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/DefaultShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"PlayerShaderCom_Proto", SCENE_TYPE::STATIC_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/PlayerShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"TerrainShaderCom_Proto", SCENE_TYPE::STAGE_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/TerrainShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"SkyBoxShader_Proto", SCENE_TYPE::STATIC_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/SkyBoxShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"EffectShader_Proto", SCENE_TYPE::STATIC_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/EffectShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"UIShader_Proto", SCENE_TYPE::STATIC_SCENE, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/UIShader.fx"))))
		return E_FAIL;


	return NOERROR;
}

// 사각형을 그리기 위한 버텍스 버퍼를 만드는 컴포넌트를 등록하는 함수
HRESULT CMainApp::AddVIBufferRectComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferRectCom_Proto", SCENE_TYPE::STATIC_SCENE, CVIBufferRect::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

// 지형을 그리기 위한 버텍스 버퍼를 만드는 컴포넌트를 등록하는 함수
HRESULT CMainApp::AddVIBufferTerrainComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferTerrainCom_Proto", SCENE_TYPE::STAGE_SCENE, CVIBufferTerrain::CreateProto(m_pGraphicDevice,129,129,1.f))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferTerrainFromBMPCom_Proto", SCENE_TYPE::STAGE_SCENE, CVIBufferTerrain::CreateProto(m_pGraphicDevice, L"../Bin/Resourece/Texture/Terrain/Height.bmp"))))
		return E_FAIL;

	return NOERROR;
}

// 텍스쳐를 로드하여 컴포넌트로 등록하는 함수
HRESULT CMainApp::AddTextureComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Black_Dot_Proto", SCENE_TYPE::STATIC_SCENE, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/black_dot.png"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"White_Dot_Proto", SCENE_TYPE::STATIC_SCENE, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/white_dot.png"))))
		return E_FAIL;

	return NOERROR;

}

// CTransform 컴포넌트를 등록하는 함수
HRESULT CMainApp::AddTransformComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, CTransform::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::AddPickingComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"PickerCom_Proto", SCENE_TYPE::STATIC_SCENE, CPicker::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"PickeeCom_Proto", SCENE_TYPE::STATIC_SCENE, CPickee::CreateProto(m_pGraphicDevice))))
		return E_FAIL;


	return NOERROR;
}

HRESULT CMainApp::AddColliderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"AABB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_AABB))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_OBB))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Sphere_Collider_Proto", SCENE_TYPE::STATIC_SCENE, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_SPHERE))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::AddSkyBoxObject(){
	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferCubeCom_Proto", SCENE_TYPE::STATIC_SCENE, CVIBufferCube::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoObject(L"SkyBox_Proto", CSkyBox::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::AddFrustumComponent(){
	if(FAILED(m_pManagement->AddProtoComponent(L"FrustumCom_Proto", SCENE_TYPE::STATIC_SCENE, CFrustum::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

// 생성 함수(static)
CMainApp * CMainApp::Create(){
	CMainApp* pInstance = new CMainApp;
	if(FAILED(pInstance->InitMainApp())){
		MSG_BOX(L"CMainApp Create Failure");
		SafeRelease(pInstance);
		return nullptr;
	}

	return pInstance;
}

// Free
void CMainApp::Free(){
	//참조하여 사용한 변수들 Release
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pManagement);


	// 유닛매니저 Release
	CUnitManager::GetInstance()->DestroyInstance();

	//매니지먼트 객체 Release함(아예 Delete 되도록)
	CManagement::ReleaseEngine();
}
