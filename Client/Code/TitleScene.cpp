// Client 헤더
#include "stdafx.h"
#include "..\Header\TitleScene.h"
#include "..\Header\UIBackground.h"
#include "..\Header\GameStartButton.h"
#include "..\Header\GameExitButton.h"
#include "..\Header\LoadingScene.h"

// Reference 헤더
#include "Management.h"


USING(Client)

// 생성자
CTitleScene::CTitleScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CScene(_pGraphicDevice){
}

// 해당 씬에 필요한 GameObject의 클론 생성(연습)
HRESULT CTitleScene::AddUIBackground(){
	CManagement* pManagement = CManagement::GetInstance();

	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);


	if(FAILED(pManagement->AddProtoComponent(L"TextureCom_Proto", SCENE_TYPE::TITLE_SCENE, CTexture::CreateProto(CScene::GetGraphicDevice(), CTexture::TEXTURE_GENERAL, L"../../Resource/Texture/bg_back.png"))))
		return E_FAIL;

	pManagement->AddProtoObject(L"UIBackground_Proto", CUIBackground::CreateProto(CScene::GetGraphicDevice()));

	CImageUI::UI_DESC tUIDesc;

	tUIDesc.fSizeX = _float(WINCX);
	tUIDesc.fSizeY = _float(WINCY);
	tUIDesc.fX = _float(WINCX>>1);
	tUIDesc.fY = _float(WINCY>>1);
	tUIDesc.szTextureName = L"TextureCom_Proto";
	tUIDesc.uiSceneNumber = TITLE_SCENE;

	if(FAILED(pManagement->AddCloneObjectToLayer(L"UIBackground_Proto", SCENE_TYPE::TITLE_SCENE, L"Title_Backgorund", &tUIDesc))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CTitleScene::AddGameStartButton(){
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

	if(FAILED(pManagement->AddProtoObject(L"GameStartButton_Proto", CGameStartButton::CreateProto(CScene::GetGraphicDevice(), &tFontDesc)))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	CTextUI::TextUI_DESC tDesc;
	tDesc.fX = 160.f;
	tDesc.fY = 500.f;
	tDesc.fSizeX = 180.f;
	tDesc.fSizeY = 50.f;
	tDesc.uiTextLength = 128;

	if(FAILED(pManagement->AddCloneObjectToLayer(L"GameStartButton_Proto", SCENE_TYPE::TITLE_SCENE, L"Title_UI", &tDesc, reinterpret_cast<CGameObject**>(&m_pStartButton)))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	SafeRelease(pManagement);

	return NOERROR;
}


HRESULT CTitleScene::AddGameExitButton(){
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

	if(FAILED(pManagement->AddProtoObject(L"GameExitButton_Proto", CGameExitButton::CreateProto(CScene::GetGraphicDevice(), &tFontDesc)))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	CTextUI::TextUI_DESC tDesc;
	tDesc.fX = 160.f;
	tDesc.fY = 560.f;
	tDesc.fSizeX = 180.f;
	tDesc.fSizeY = 50.f;
	tDesc.uiTextLength = 128;

	if(FAILED(pManagement->AddCloneObjectToLayer(L"GameExitButton_Proto", SCENE_TYPE::TITLE_SCENE, L"Title_UI", &tDesc, reinterpret_cast<CGameObject**>(&m_pExitButton)))){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	SafeRelease(pManagement);

	return NOERROR;
}

void CTitleScene::ChangeScene(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return;
	SafeAddRef(pManagement);

	CScene* pScene = CLoadingScene::Create(CScene::GetGraphicDevice());

	CManagement::GetInstance()->SceneClear(TITLE_SCENE);
	CManagement::GetInstance()->SetCurrentScene(pScene);

	pScene->Release();

	SafeRelease(pManagement);
}

// Initialize
HRESULT CTitleScene::InitScene(){
	if(FAILED(AddUIBackground()))
		return E_FAIL;

	if(FAILED(AddGameExitButton()))
		return E_FAIL;

	if(FAILED(AddGameStartButton()))
		return E_FAIL;

	return NOERROR;
}

// Update
HRESULT CTitleScene::UpdateScene(_double _dDeltaTime){

	return NOERROR;
}

// LateUpdate
HRESULT CTitleScene::LateUpdateScene(_double _dDeltaTime){
	if(m_pExitButton->IsClick())
		::DestroyWindow(g_hWnd);
	else if(m_pStartButton->IsClick())
		ChangeScene();

	return NOERROR;
}

// Render
HRESULT CTitleScene::RenderScene(){
	return NOERROR;
}


// 생성 함수(static)
CTitleScene * CTitleScene::Create(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CTitleScene* pInstance = new CTitleScene(_pGraphicDevice);
	if(FAILED(pInstance->InitScene())){
		MSG_BOX(L"CTitleScene Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Free
void CTitleScene::Free(){

	CScene::Free();
}