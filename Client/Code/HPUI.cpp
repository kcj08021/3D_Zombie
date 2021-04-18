#include "stdafx.h"

#include "..\Header\HPUI.h"
#include "..\Header\UnitManager.h"

USING(Client)

CHPUI::CHPUI(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CTextUI(_pGraphicDevice){
}

CHPUI::CHPUI(const CHPUI & _HPUI) 
	: CTextUI(_HPUI){
}

HRESULT CHPUI::InitGameObject_Proto(D3DXFONT_DESC * _pDesc){
	return CTextUI::InitGameObject_Proto(_pDesc);
}

HRESULT CHPUI::InitGameObject_Clone(void * _pArgument){
	return CTextUI::InitGameObject_Clone(_pArgument);
}

HRESULT CHPUI::LateInitGameObject(){
	GetPlayerHP();

	return NOERROR;
}

HRESULT CHPUI::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CHPUI::LateUpdateGameObject(_double _dDeltaTime){
	GetPlayerHP();

	return CTextUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CHPUI::RenderGameObject(_double _dDeltaTime){
	return CTextUI::RenderGameObject(_dDeltaTime);
}

void CHPUI::GetPlayerHP(){
	_int fPlayerHP = CUnitManager::GetInstance()->GetPlayerHP();

	_tchar szPlayerHp[64] = {};

	_itot_s(fPlayerHP, szPlayerHp, 10);

	lstrcpy(m_pString, szPlayerHp);
}

CHPUI * CHPUI::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc){
	CHPUI* pInstance = new CHPUI(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pDesc))){
		MSG_BOX(L"CHPUI Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CHPUI::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CHPUI(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CHPUI Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CHPUI::Free(){
	CTextUI::Free();
}
