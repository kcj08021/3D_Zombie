// Client 헤더
#include "stdafx.h"
#include "..\Header\StageScene.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\StaticMeshLoader.h"
#include "..\Header\FreeCamera.h"
#include "..\Header\Terrain.h"
#include "..\Header\SkyBox.h"
#include "..\Header\StaticMeshObject.h"

#include "..\Header\Player.h"
#include "..\Header\M16M203.h"
#include "..\Header\AK47.h"
#include "..\Header\G3.h"
#include "..\Header\RPD.h"
#include "..\Header\M16M203_Grenade.h"
#include "..\Header\Grenade.h"

#include "..\Header\WeaponObject.h"

#include "..\Header\NonPlayer.h"


#include "..\Header\MuzzleEffect.h"
#include "..\Header\BloodEffect.h"

#include "..\Header\AmmoHUD.h"
#include "..\Header\HPUI.h"
#include "..\Header\WeaponChangeTextUI.h"

#include "..\Header\UnitManager.h"

// Reference 헤더
#include "Management.h"

USING(Client)

// 생성자
CStageScene::CStageScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CScene(_pGraphicDevice)
	, m_pSceneMap(nullptr)
	, m_bIsDebug(false)
	, m_pNavigationCom(nullptr)
	, m_pUnitManager(nullptr){
}

// Initialize
HRESULT CStageScene::InitScene(){
	m_pUnitManager = CUnitManager::GetInstance();
	if(nullptr == m_pUnitManager)
		return E_FAIL;
	SafeAddRef(m_pUnitManager);

	m_pUnitManager->SetCurrentSceanNum(STAGE_SCENE);

	if(FAILED(AddStaticMesh()))
		return E_FAIL;

	if(FAILED(AddTerrain()))
		return E_FAIL;

	if(FAILED(AddLightDesc()))
		return E_FAIL;

	if(FAILED(AddEffect()))
		return E_FAIL;

	if(FAILED(AddPlayer()))
		return E_FAIL;

	if(FAILED(AddNonPlayer()))
		return E_FAIL;

	if(FAILED(AddStaticMeshLoader()))
		return E_FAIL;

	if(FAILED(AddUI()))
		return E_FAIL;

	return NOERROR;
}

// Update
HRESULT CStageScene::UpdateScene(_double _dDeltaTime){
		CManagement* pManagement = CManagement::GetInstance();
		if(nullptr == pManagement)
			return E_FAIL;

		SafeAddRef(pManagement);

		if(pManagement->IsKeyDown(DIK_F2))
			m_bIsDebug = !m_bIsDebug;

		SafeRelease(pManagement);

		m_pUnitManager->ResponNonPlayer(_dDeltaTime);

	return NOERROR;
}
// LateUpdate
HRESULT CStageScene::LateUpdateScene(_double _dDeltaTime){
	return NOERROR;
}

// Render
HRESULT CStageScene::RenderScene(){
	if(m_bIsDebug){
		CManagement* pManagement = CManagement::GetInstance();
		if(nullptr == pManagement)
			return E_FAIL;
		SafeAddRef(pManagement);

		m_pNavigationCom->RenderNaviMash();
		pManagement->RenderCollider();

		SafeRelease(pManagement);
	}

	return NOERROR;
}

// 프리카메라 오브젝트를 추가하는 함수
HRESULT CStageScene::AddFreeCamera(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;
	// 카메라의 세부 설정을 전달해줄 구조체 생성 및 초기화
	CFreeCamera::CAMERA_DESC tCameraDesc = {};
	tCameraDesc.vEye = _vec3(276.f, 1.7f, -40.f);
	tCameraDesc.vAt = _vec3(0.f, 1.8f, 0.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);

	tCameraDesc.fFovY = D3DXToRadian(60.f);
	tCameraDesc.fAspect = WINCX / static_cast<_float>(WINCY);
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFar = 200.f;

	tCameraDesc.tTransformDesc = { nullptr, 15.f, D3DXToRadian(90.f) };
	
	pManagement->AddProtoObject(L"FreeCamera_Proto", CFreeCamera::CreateProto(CScene::GetGraphicDevice()));
	hr = pManagement->AddCloneObjectToLayer(L"FreeCamera_Proto", STAGE_SCENE, L"CameraLayer", &tCameraDesc);

	SafeRelease(pManagement);

	return hr;
}

// 지형 오브젝트를 추가하는 함수
HRESULT CStageScene::AddTerrain(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;

	_matrix mat = {};
	D3DXMatrixIdentity(&mat);
	//D3DXMatrixRotationX(&mat, D3DXToRadian(90.f));

	pManagement->AddProtoObject(L"Terrain_Proto", CTerrain::CreateProto(CScene::GetGraphicDevice()));

	m_pNavigationCom = dynamic_cast<CNavigation*>(pManagement->AddCloneComponent(L"Navigation_Proto", SCENE_TYPE::STAGE_SCENE));

	hr = pManagement->AddCloneObjectToLayer(L"Terrain_Proto", STAGE_SCENE, L"TerrainLayer", nullptr,reinterpret_cast<CGameObject**>(&m_pSceneMap));

	CSkyBox::SKYBOX_DESC tDesc;
	tDesc.szTextureComTag = L"SkyBoxTextureCom_Proto";

	pManagement->AddCloneObjectToLayer(L"SkyBox_Proto", STAGE_SCENE, L"SkyboxLayer", &tDesc);

	SafeRelease(pManagement);

	return hr;
}

HRESULT CStageScene::AddLightDesc(){

	CManagement*		pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	D3DLIGHT9 LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.Direction = _vec3(-1.f, -1.f, 0.f);

	if(FAILED(pManagement->AddLight(GetGraphicDevice(), &LightDesc))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Position = _vec3(-13.f, 10.f, 12.f);
	LightDesc.Range = 10.f;

	if(FAILED(pManagement->AddLight(GetGraphicDevice(), &LightDesc))){
		SafeRelease(pManagement);
		return E_FAIL;
	}
	
	SafeRelease(pManagement);

	return NOERROR;
}


HRESULT CStageScene::AddStaticMesh(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	pManagement->AddProtoObject(L"StaticMeshObject_Proto", CStaticMeshObject::CreateProto(CScene::GetGraphicDevice()));

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CStageScene::AddPlayer(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;

	pManagement->AddProtoComponent(L"M16M203_Proto", STAGE_SCENE, Cm16m203::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddProtoComponent(L"AK47_Proto", STAGE_SCENE, Cak47::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddProtoComponent(L"G3_Proto", STAGE_SCENE, CG3::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddProtoComponent(L"RPD_Proto", STAGE_SCENE, CRPD::CreateProto(CScene::GetGraphicDevice()));

	pManagement->AddProtoComponent(L"M16M203_Grenade_Proto", STAGE_SCENE, CM16M203_Grenade::CreateProto(CScene::GetGraphicDevice()));

	pManagement->AddProtoObject(L"Player_Proto", CPlayer::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddProtoObject(L"Grenade_Proto", CGrenade::CreateProto(CScene::GetGraphicDevice()));
	
	CCamera::CAMERA_DESC tCameraDesc = {};
	tCameraDesc.vEye = _vec3(-15.f, 7.0f, 15.f);
	tCameraDesc.vAt = _vec3(0.f, 7.0f, 0.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);

	tCameraDesc.fFovY = D3DXToRadian(60.f);
	tCameraDesc.fAspect = WINCX / static_cast<_float>(WINCY);
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFar = 200.0f;

	tCameraDesc.tTransformDesc = { nullptr, 4.f, D3DXToRadian(90.f) };

	CPlayer::PLAYER_DESC tPlayerDesc = { tCameraDesc };

	hr = pManagement->AddCloneObjectToLayer(L"Player_Proto", STAGE_SCENE, L"PlayerLayer", &tPlayerDesc);

	pManagement->AddProtoObject(L"WeaponObject_Proto", CWeaponObject::CreateProto(CScene::GetGraphicDevice(), dynamic_cast<CStaticMesh*>(m_pSceneMap->GetComponent(L"map"))));
	
	SafeRelease(pManagement);

	return hr;
}

HRESULT CStageScene::AddNonPlayer(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	if(FAILED(pManagement->AddProtoObject(L"3rd_Model_Proto", CNonPlayer::CreateProto(CScene::GetGraphicDevice())))){
		SafeRelease(pManagement);
	return E_FAIL;
	}

	m_pUnitManager->SetResponPos(_vec3(0.f, 7.f, 0.f));
	m_pUnitManager->SetResponPos(_vec3(15.f, 7.f, 15.f));
	m_pUnitManager->SetResponPos(_vec3(15.f, 7.f, -15.f));
	m_pUnitManager->SetResponPos(_vec3(-15.f, 7.f, -15.f));

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CStageScene::AddStaticMeshLoader(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	pManagement->AddProtoComponent(L"StaticMeshLoader_Proto", SCENE_TYPE::STAGE_SCENE, CStaticMeshLoader::CreateProto(CScene::GetGraphicDevice(),L"../../Data/StaticObjectData.STTOBJ", STAGE_SCENE));

	m_pStaticMeshLoader = dynamic_cast<CStaticMeshLoader*>(pManagement->AddCloneComponent(L"StaticMeshLoader_Proto", STAGE_SCENE));
	if(nullptr != m_pStaticMeshLoader)
		m_pStaticMeshLoader->CreateStaticMeshObject();

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CStageScene::AddEffect(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);


	CEffect::EFFECT_DESC tDesc;
	tDesc.eType = CEffect::EFFECT_TYPE::RANDOM;
	tDesc.uiEffectPoolCount = 1;
	tDesc.szTextureName = L"MuzzleFlash_Texture_Proto";
	tDesc.uiSceneNumber = STAGE_SCENE;
	tDesc.uiTextureCount = 4;
	tDesc.fEffectPlayTime = 0.f;
	tDesc.szShaderFileName = nullptr;
	tDesc.szShaderPassNum = 0;

	_matrix tWorldMatrix,matScale,matTrans;
	D3DXMatrixIdentity(&tWorldMatrix);
	D3DXMatrixScaling(&matScale, 0.5f, 0.5f, 0.5f);
	D3DXMatrixTranslation(&matTrans, 0.f, 7.f, 0.f);
	tWorldMatrix = matScale * matTrans;
	tDesc.pWorldMatrix = &tWorldMatrix;

	pManagement->AddProtoObject(L"Muzzle_Effect_Proto", CMuzzleEffect::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddCloneObjectToLayer(L"Muzzle_Effect_Proto", STAGE_SCENE, L"MuzzleEffect", &tDesc);


	tDesc.eType = CEffect::EFFECT_TYPE::ANIMATION;
	tDesc.szTextureName = L"Blood_Texture_Proto";
	tDesc.uiEffectPoolCount = 30;
	tDesc.fMaxFrame = 4;
	tDesc.uiSceneNumber = STAGE_SCENE;
	tDesc.uiTextureCount = 1;
	tDesc.fEffectPlayTime = .2f;
	tDesc.szShaderFileName = L"ClientEffectShader_Proto";
	tDesc.szShaderPassNum = 0;

	D3DXMatrixIdentity(&tWorldMatrix);
	D3DXMatrixScaling(&matScale, .5f, .5f, .5f);
	D3DXMatrixTranslation(&matTrans, 0.f, 7.f, 0.f);
	tWorldMatrix = matScale * matTrans;
	tDesc.pWorldMatrix = &tWorldMatrix;

	CGameObject* pGameObject = nullptr;

	pManagement->AddProtoObject(L"Boold_Effect_Proto", CBloodEffect::CreateProto(CScene::GetGraphicDevice()));
	pManagement->AddCloneObjectToLayer(L"Boold_Effect_Proto", STAGE_SCENE, L"BloodEffect", &tDesc, &pGameObject);

	reinterpret_cast<CEffect*>(pGameObject)->SetEffectStatus(true);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CStageScene::AddUI(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	D3DXFONT_DESC tFontDesc;
	tFontDesc.Height = 40;
	tFontDesc.Width = 20;
	tFontDesc.Weight = FW_NORMAL;
	tFontDesc.MipLevels = 1;
	tFontDesc.Italic = FALSE;
	tFontDesc.CharSet = HANGEUL_CHARSET;
	tFontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	tFontDesc.Quality = DEFAULT_QUALITY;
	tFontDesc.PitchAndFamily = FF_DONTCARE;
	lstrcpyW(tFontDesc.FaceName, L"");

	pManagement->AddProtoObject(L"AMMO_HUD_Proto", CAmmoHUD::CreateProto(CScene::GetGraphicDevice(), &tFontDesc));

	CTextUI::TextUI_DESC tDesc;
	tDesc.fX = 1140.f;
	tDesc.fY = 680.f;
	tDesc.fSizeX = 300.f;
	tDesc.fSizeY = 100.f;
	tDesc.uiTextLength = 128;

	pManagement->AddCloneObjectToLayer(L"AMMO_HUD_Proto", SCENE_TYPE::STAGE_SCENE, L"Layer_UI", &tDesc);

	pManagement->AddProtoObject(L"HPUI_Proto", CHPUI::CreateProto(CScene::GetGraphicDevice(), &tFontDesc));

	tDesc.fX = 140.f;
	tDesc.fY = 680.f;
	tDesc.fSizeX = 300.f;
	tDesc.fSizeY = 100.f;
	tDesc.uiTextLength = 128;

	pManagement->AddCloneObjectToLayer(L"HPUI_Proto", SCENE_TYPE::STAGE_SCENE, L"Layer_UI", &tDesc);

	tFontDesc.Height = 20;
	tFontDesc.Width = 10;
	tFontDesc.Weight = FW_NORMAL;
	tFontDesc.MipLevels = 1;
	tFontDesc.Italic = FALSE;
	tFontDesc.CharSet = HANGEUL_CHARSET;
	tFontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	tFontDesc.Quality = DEFAULT_QUALITY;
	tFontDesc.PitchAndFamily = FF_DONTCARE;
	lstrcpyW(tFontDesc.FaceName, L"");

	pManagement->AddProtoObject(L"WeaponChangeUI_Proto", CWeaponChangeTextUI::CreateProto(CScene::GetGraphicDevice(), &tFontDesc));

	tDesc.fX = 720.f;
	tDesc.fY = 520.f;
	tDesc.fSizeX = 300.f;
	tDesc.fSizeY = 150.f;
	tDesc.uiTextLength = 128;
	
	pManagement->AddCloneObjectToLayer(L"WeaponChangeUI_Proto", SCENE_TYPE::STAGE_SCENE, L"Layer_UI", &tDesc);

	SafeRelease(pManagement);

	return NOERROR;
}

// 생성 함수(static)
CStageScene * CStageScene::Create(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CStageScene* pInstance = new CStageScene(_pGraphicDevice);
	if(FAILED(pInstance->InitScene())){
		MSG_BOX(L"CStageScene Init Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Free
void CStageScene::Free(){
	SafeRelease(m_pNavigationCom);
	SafeRelease(m_pStaticMeshLoader);
	SafeRelease(m_pUnitManager);

	CScene::Free();
}
