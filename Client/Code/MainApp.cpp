// Client ���
#include "stdafx.h"
#include "..\Header\MainApp.h"
#include "..\Header\TitleScene.h"
#include "..\Header\StageScene.h"
#include "..\Header\LoadingScene.h"
#include "..\Header\SkyBox.h"
#include "..\Header\UnitManager.h"

USING(Client)

// ������
CMainApp::CMainApp()
	: m_pManagement(nullptr)
	, m_pGraphicDevice(nullptr)
	, m_pRendererCom(nullptr)
	, m_bIsInputStateUpdateFailure(false){
}

// Initialize
HRESULT CMainApp::InitMainApp(){
	// ����Ʈ�� ����
	if(FAILED(SettingDefault(CGraphicDevice::MODE_WINDOWED, WINCX, WINCY)))
		return E_FAIL;
	// ������ ������Ʈ ���
	if(FAILED(AddRenderComponent()))
		return E_FAIL;
	// ���̴� ������Ʈ ���
	if(FAILED(AddShaderComponent()))
		return E_FAIL;
	// �簢�� ���ؽ� ���� ������Ʈ ���
	if(FAILED(AddVIBufferRectComponent()))
		return E_FAIL;
	// �ؽ��� ������Ʈ ���
	if(FAILED(AddTextureComponent()))
		return E_FAIL;
	// Transform ������Ʈ ���
	if(FAILED(AddTransformComponent()))
		return E_FAIL;
	// ��ŷ ���� ������Ʈ ���
	if(FAILED(AddPickingComponent()))
		return E_FAIL;
	// �ݶ��̴� ������Ʈ ���
	if(FAILED(AddColliderComponent()))
		return E_FAIL;
	if(FAILED(AddSkyBoxObject()))
		return E_FAIL;

	if(FAILED(AddFrustumComponent()))
		return E_FAIL;

	// �� ����
	if(FAILED(SettingScene(SCENE_TYPE::TITLE_SCENE)))
		return E_FAIL;

	return NOERROR;
}

// Update
HRESULT CMainApp::UpdateMainApp(_double _dDeltaTime){
	if(nullptr == m_pManagement)
		return E_FAIL;

	// �Է� ���� ������Ʈ 
	if(FAILED(m_pManagement->UpdateInputState()))
		if(!m_bIsInputStateUpdateFailure){
			MSG_BOX(L"CInputDevice Update Failure");
			m_bIsInputStateUpdateFailure = true;
		}

	if(m_pManagement->IsKeyDown(DIK_F2))
		m_pRendererCom->SetRenderDebugBuffer(!m_pRendererCom->GetRenderDebugBuffer());

	// �Ŵ�����Ʈ���� �����ǰ� �ִ� ���� ���� ������Ʈ
	return m_pManagement->UpdateCurrentScene(_dDeltaTime);
}

// Render
HRESULT CMainApp::RenderMainApp(_double _dDeltaTime){
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;

	if(nullptr == m_pManagement)
		return E_FAIL;

	// �׷��� ����̽��� �ĸ���� ����
	m_pGraphicDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	// �ĸ���ۿ� �׸��� ����
	m_pGraphicDevice->BeginScene();

	// ������ ������Ʈ���� ���� ����
	m_pRendererCom->DoRender(_dDeltaTime);
	// �Ŵ�����Ʈ���� �����ǰ� �ִ� ���� ���� ����
	m_pManagement->RenderCurrentScene();

	// �ĸ� ���ۿ� �׸��� ��
	m_pGraphicDevice->EndScene();
	// ���� �׷��� �ĸ� ���۸� ���� ���۷� ��ü �� �ÿ�
	m_pGraphicDevice->Present(nullptr, nullptr, 0, nullptr);

	return NOERROR;
}

// �ʱ⿡ �⺻���� ��� �������� �����ϴ� �Լ�
HRESULT CMainApp::SettingDefault(CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY){
	// �Ŵ�����Ʈ ��ü �غ�
	if(nullptr == m_pManagement){
		m_pManagement = CManagement::GetInstance();
		if(nullptr == m_pManagement)
			return E_FAIL;

		SafeAddRef(m_pManagement);
	}

	// �������� ����� ��ü���� �غ�
	if(FAILED(m_pManagement->InitEngine(SCENE_END)))
		return E_FAIL;

	// �׷��� ��ġ �غ�
	if(FAILED(m_pManagement->InitGraphicDevice(g_hWnd, _eMode, _uiWinCX, _uiWinCY, &m_pGraphicDevice)))
		return E_FAIL;

	// �Է� ��ġ �غ�
	if(FAILED(m_pManagement->InitInputDevice(g_hInst, g_hWnd)))
		return E_FAIL;

	// ���� �غ�
	if(FAILED(m_pManagement->InitSound()))
		return E_FAIL;

	return NOERROR;
}

// ���� �����ϴ� �Լ�
HRESULT CMainApp::SettingScene(SCENE_TYPE _eSceneType){
	CScene* pScene = nullptr;

	// �Ű� ������ �޾ƿ� �� Ÿ�Կ� ���� �� ����
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

	// ������ ������ ��ü
	if(FAILED(m_pManagement->SetCurrentScene(pScene)))
		return E_FAIL;

	pScene->Release();

	return NOERROR;
}

// ������ ������Ʈ�� ����ϴ� �Լ�
HRESULT CMainApp::AddRenderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE, m_pRendererCom = CRenderer::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	SafeAddRef(m_pRendererCom);

	return NOERROR;
}

// ���̴� ������Ʈ�� ����ϴ� �Լ�
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

// �簢���� �׸��� ���� ���ؽ� ���۸� ����� ������Ʈ�� ����ϴ� �Լ�
HRESULT CMainApp::AddVIBufferRectComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferRectCom_Proto", SCENE_TYPE::STATIC_SCENE, CVIBufferRect::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

// ������ �׸��� ���� ���ؽ� ���۸� ����� ������Ʈ�� ����ϴ� �Լ�
HRESULT CMainApp::AddVIBufferTerrainComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferTerrainCom_Proto", SCENE_TYPE::STAGE_SCENE, CVIBufferTerrain::CreateProto(m_pGraphicDevice,129,129,1.f))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"VIBufferTerrainFromBMPCom_Proto", SCENE_TYPE::STAGE_SCENE, CVIBufferTerrain::CreateProto(m_pGraphicDevice, L"../Bin/Resourece/Texture/Terrain/Height.bmp"))))
		return E_FAIL;

	return NOERROR;
}

// �ؽ��ĸ� �ε��Ͽ� ������Ʈ�� ����ϴ� �Լ�
HRESULT CMainApp::AddTextureComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Black_Dot_Proto", SCENE_TYPE::STATIC_SCENE, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/black_dot.png"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"White_Dot_Proto", SCENE_TYPE::STATIC_SCENE, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/white_dot.png"))))
		return E_FAIL;

	return NOERROR;

}

// CTransform ������Ʈ�� ����ϴ� �Լ�
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

// ���� �Լ�(static)
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
	//�����Ͽ� ����� ������ Release
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pManagement);


	// ���ָŴ��� Release
	CUnitManager::GetInstance()->DestroyInstance();

	//�Ŵ�����Ʈ ��ü Release��(�ƿ� Delete �ǵ���)
	CManagement::ReleaseEngine();
}
