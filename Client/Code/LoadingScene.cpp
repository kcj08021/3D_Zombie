#include "stdafx.h"

// Client 헤더
#include "..\Header\Client_Headers.h"
#include "..\Header\LoadingScene.h"
#include "..\Header\UIBackground.h"
#include "..\Header\LoadingProgress.h"
#include "..\Header\StageScene.h"

// Reference 헤더
#include "Management.h"

USING(Client)

CLoadingScene::CLoadingScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CScene(_pGraphicDevice){
}

HRESULT CLoadingScene::InitScene(){
	CManagement* pManagement = CManagement::GetInstance();

	::ShowCursor(false);

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 1, ThreadMain, this, 0, nullptr);
	if(0 == m_hThread)
		return E_FAIL;

	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	CImageUI::UI_DESC tUIDesc;

	tUIDesc.fSizeX = _float(WINCX);
	tUIDesc.fSizeY = _float(WINCY) - 30.f;
	tUIDesc.fX = _float(WINCX >> 1);
	tUIDesc.fY = _float(WINCY >> 1) - 15.f;
	tUIDesc.szTextureName = L"Black_Dot_Proto";
	tUIDesc.uiSceneNumber = STATIC_SCENE;

	if(FAILED(pManagement->AddCloneObjectToLayer(L"UIBackground_Proto", SCENE_TYPE::LOADING_SCENE, L"LoadingBackground", &tUIDesc)))
		return E_FAIL;

	if(pManagement->AddProtoObject(L"LoadingProgress_Proto", CLoadingProgress::CreateProto(CScene::GetGraphicDevice())))
		return E_FAIL;

	tUIDesc.fSizeX = _float(WINCX);
	tUIDesc.fSizeY = 30.f;
	tUIDesc.fX = _float(WINCX >> 1);
	tUIDesc.fY = 705.f;
	tUIDesc.szTextureName = L"White_Dot_Proto";
	tUIDesc.uiSceneNumber = STATIC_SCENE;

	if(FAILED(pManagement->AddCloneObjectToLayer(L"LoadingProgress_Proto", SCENE_TYPE::LOADING_SCENE, L"LoadingProgress", &tUIDesc, reinterpret_cast<CGameObject**>(&m_pLoadingProgress))))
		return E_FAIL;

	SafeAddRef(m_pLoadingProgress);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CLoadingScene::UpdateScene(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CLoadingScene::LateUpdateScene(_double _dDeltaTime){
	if(m_fLoadProgress < m_uiLoadProgress)
		m_fLoadProgress += m_uiLoadProgress*(_float)_dDeltaTime;

	m_pLoadingProgress->SetLoadingProgress(m_fLoadProgress / 24.f);

	if(m_uiLoadProgress == 24){
		CScene* pScene = CStageScene::Create(CScene::GetGraphicDevice());

		CManagement::GetInstance()->SceneClear(LOADING_SCENE);
		CManagement::GetInstance()->SetCurrentScene(pScene);
		
		pScene->Release();
	}
	return NOERROR;
}

HRESULT CLoadingScene::RenderScene(){
	return NOERROR;
}

_uint CLoadingScene::ThreadMain(void * pArg){
	CLoadingScene*	pLoadingScene = (CLoadingScene*)pArg;

	EnterCriticalSection(&pLoadingScene->m_CS);

	HRESULT	hr = 0;

	hr = pLoadingScene->LoadingObject();

	LeaveCriticalSection(&pLoadingScene->m_CS);

	if(FAILED(hr))
		return -1;

	return 0;
}

HRESULT CLoadingScene::LoadingObject(){
	CManagement*	pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	// 맵 관련
	pManagement->AddProtoComponent(L"map_proto", SCENE_TYPE::STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/shipment/", L"shipment.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"Navigation_Proto", SCENE_TYPE::STAGE_SCENE, CNavigation::CreateProto(CScene::GetGraphicDevice(), L"../../Data/Navigation.dat"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"SkyBoxTextureCom_Proto", STAGE_SCENE, CTexture::CreateProto(CScene::GetGraphicDevice(), CTexture::TEXTURE_CUBE, L"../../Resource/Texture/Skybox/sp_boga_ft.dds"));
	++m_uiLoadProgress;


	// 1인칭 시점 관련
	_matrix matTrans, matScale, matRotate;
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);

	D3DXMatrixRotationY(&matRotate, D3DXToRadian(-90.f));
	matTrans = matScale* matRotate;

	pManagement->AddProtoComponent(L"viewmodel_ak47_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_ak47/", L"viewmodel_ak47.x", &matTrans));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"viewmodel_g3_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_g3/", L"viewmodel_g3.x", &matTrans));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"viewmodel_m16m203_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_m16m203/", L"viewmodel_m16m203.x", &matTrans));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"viewmodel_rpd_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_rpd/", L"viewmodel_rpd.x", &matTrans));
	++m_uiLoadProgress;

	pManagement->AddProtoComponent(L"viewmodel_m16m203_grenade_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_m16m203_grenade/", L"viewmodel_m16m203_grenade.x", &matTrans));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"viewmodel_m67_grenade_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/viewmodel_m67_grenade/", L"viewmodel_m67_grenade.x", &matTrans));
	++m_uiLoadProgress;

	// 3인칭 관련
	pManagement->AddProtoComponent(L"weapon_ak47_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/weapon_ak47/", L"weapon_ak47.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"weapon_g3_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/weapon_g3/", L"weapon_g3.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"weapon_m16m203_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/weapon_m16m203/", L"weapon_m16m203.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"weapon_rpd_Proto", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/weapon_rpd/", L"weapon_rpd.x"));
	++m_uiLoadProgress;


	// 맵 오브젝트 관련
	D3DXMatrixIdentity(&matTrans);

	D3DXMatrixRotationY(&matRotate, D3DXToRadian(-90.f));
	matTrans = matScale* matRotate;

	D3DXMatrixScaling(&matScale, 2.f, 2.f, 2.f);

	pManagement->AddProtoComponent(L"viewmodel_knife_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/viewmodel_knife/", L"viewmodel_knife.x", &matScale));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"projectile_m67_grenade_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/projectile_m67_grenade/", L"projectile_m67_grenade.x", &matRotate));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"projectile_m203_grenade_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/projectile_m203_grenade/", L"projectile_m203_grenade.x", &matRotate));
	++m_uiLoadProgress;

	pManagement->AddProtoComponent(L"car1_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car1/", L"car1.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"car2_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car2/", L"car2.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"car3_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car3/", L"car3.x"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"car4_Com_Proto", STAGE_SCENE, CStaticMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car4/", L"car4.x", &matTrans));
	++m_uiLoadProgress;



	// NPC 관련
	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);

	D3DXMatrixRotationY(&matRotate, D3DXToRadian(-90.f));
	matTrans = matScale* matRotate;

	pManagement->AddProtoComponent(L"zombie", STAGE_SCENE, CDynamicMesh::CreateProto(CScene::GetGraphicDevice(), L"../../Resource/Meshes/DynamicMesh/zombie/", L"zombie.X", &matTrans));
	++m_uiLoadProgress;


	// 이펙트 관련
	pManagement->AddProtoComponent(L"ClientEffectShader_Proto", SCENE_TYPE::STAGE_SCENE, CShader::CreateProto(CScene::GetGraphicDevice(), L"../Bin/ShaderFile/ClientEffectShader.fx"));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"MuzzleFlash_Texture_Proto", STAGE_SCENE, CTexture::CreateProto(CScene::GetGraphicDevice(), CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/Effect/Muzzle/explosion_flash_atlas_0%d.png", 4));
	++m_uiLoadProgress;
	pManagement->AddProtoComponent(L"Blood_Texture_Proto", STAGE_SCENE, CTexture::CreateProto(CScene::GetGraphicDevice(), CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/Effect/Blood/blood_burst_atlas.dds"));
	++m_uiLoadProgress;


	SafeRelease(pManagement);

	return NOERROR;
}

CLoadingScene * CLoadingScene::Create(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CLoadingScene* pInstance = new CLoadingScene(_pGraphicDevice);
	if(FAILED(pInstance->InitScene())){
		MSG_BOX(L"CLoadingScene Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CLoadingScene::Free(){
	WaitForSingleObject(m_hThread, INFINITY);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_CS);

	SafeRelease(m_pLoadingProgress);

	CScene::Free();
}
