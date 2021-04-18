#include "stdafx.h"
#include "..\Header\UIBackground.h"

USING(Client)

CUIBackground::CUIBackground(LPDIRECT3DDEVICE9 m_pGraphicDevice)
	: CImageUI(m_pGraphicDevice){
}

CUIBackground::CUIBackground(const CUIBackground & _UIBackground)
	: CImageUI(_UIBackground){
}

HRESULT CUIBackground::InitGameObject_Proto(){
	return CImageUI::InitGameObject_Proto();
}

HRESULT CUIBackground::InitGameObject_Clone(void * _pArgument){
	return CImageUI::InitGameObject_Clone(_pArgument);
}

HRESULT CUIBackground::LateInitGameObject(){
	return NOERROR;
}

HRESULT CUIBackground::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CUIBackground::LateUpdateGameObject(_double _dDeltaTime){
	return CImageUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CUIBackground::RenderGameObject(_double _dDeltaTime){
	return CImageUI::RenderGameObject(_dDeltaTime);
}

CUIBackground * CUIBackground::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CUIBackground* pInstance = new CUIBackground(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CUIBackground Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CUIBackground::CreateClone(void * _pArgument){
	CUIBackground* pInstance = new CUIBackground(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CUIBackground Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CUIBackground::Free(){
	CImageUI::Free();
}
