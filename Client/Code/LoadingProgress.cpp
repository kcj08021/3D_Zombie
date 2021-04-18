#include "stdafx.h"
#include "..\Header\LoadingProgress.h"

USING(Client)

CLoadingProgress::CLoadingProgress(LPDIRECT3DDEVICE9 m_pGraphicDevice)
	: CImageUI(m_pGraphicDevice){
}

CLoadingProgress::CLoadingProgress(const CLoadingProgress & _LoadingProgress)
	: CImageUI(_LoadingProgress){
}

HRESULT CLoadingProgress::InitGameObject_Proto(){
	SetShaderPass(1);

	return CImageUI::InitGameObject_Proto();
}

HRESULT CLoadingProgress::InitGameObject_Clone(void * _pArgument){
	return CImageUI::InitGameObject_Clone(_pArgument);
}

HRESULT CLoadingProgress::LateInitGameObject(){
	return NOERROR;
}

HRESULT CLoadingProgress::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CLoadingProgress::LateUpdateGameObject(_double _dDeltaTime){
	return CImageUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CLoadingProgress::RenderGameObject(_double _dDeltaTime){
	this->SettingShader();

	return CImageUI::RenderGameObject(_dDeltaTime);
}

HRESULT CLoadingProgress::SettingShader(){
	if(FAILED(CImageUI::SettingShader()))
		return E_FAIL;
	
	CImageUI::SetShaderValue("g_fLoadingProgress" ,&m_fLoadingProgress, sizeof(_float));

	return NOERROR;
}

CLoadingProgress * CLoadingProgress::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CLoadingProgress* pInstance = new CLoadingProgress(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CLoadingProgress Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingProgress::CreateClone(void * _pArgument){
	CLoadingProgress* pInstance = new CLoadingProgress(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CLoadingProgress Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CLoadingProgress::Free(){
	CImageUI::Free();
}
