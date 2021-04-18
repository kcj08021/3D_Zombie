#include "stdafx.h"
// Client
#include "..\Header\WeaponChangeTextUI.h"
#include "..\Header\UnitManager.h"

USING(Client)


CWeaponChangeTextUI::CWeaponChangeTextUI(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CTextUI(_pGraphicDevice){
}

CWeaponChangeTextUI::CWeaponChangeTextUI(const CWeaponChangeTextUI & _WeaponChangeTextUI)
	: CTextUI(_WeaponChangeTextUI){
}

HRESULT CWeaponChangeTextUI::InitGameObject_Proto(D3DXFONT_DESC * _pDesc){
	return CTextUI::InitGameObject_Proto(_pDesc);
}

HRESULT CWeaponChangeTextUI::InitGameObject_Clone(void * _pArgument){

	return CTextUI::InitGameObject_Clone(_pArgument);
}

HRESULT CWeaponChangeTextUI::LateInitGameObject(){
	return CTextUI::LateInitGameObject();
}

HRESULT CWeaponChangeTextUI::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CWeaponChangeTextUI::LateUpdateGameObject(_double _dDeltaTime){	
	SettingUIEnable();

	return CTextUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CWeaponChangeTextUI::RenderGameObject(_double _dDeltaTime){
	return CTextUI::RenderGameObject(_dDeltaTime);
}

void CWeaponChangeTextUI::SettingUIEnable(){
	CUnitManager* pUnitManager = CUnitManager::GetInstance();
	if(nullptr == pUnitManager){
		m_bIsUIEnable = false;
		return;
	}

	SafeAddRef(pUnitManager);

	if((m_bIsUIEnable = pUnitManager->IsCollisionGroundWeapon())){
		lstrcpy(m_pString, L"F를 눌러 무기 변경:\n");
		lstrcat(m_pString, pUnitManager->GetGroundWeaponName());
	}

	SafeRelease(pUnitManager);
}

CWeaponChangeTextUI * CWeaponChangeTextUI::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC * _pDesc){
	CWeaponChangeTextUI* pInstance = new CWeaponChangeTextUI(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pDesc))){
		MSG_BOX(L"CWeaponChangeTextUI Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CWeaponChangeTextUI::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CWeaponChangeTextUI(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CWeaponChangeTextUI Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CWeaponChangeTextUI::Free(){
	CTextUI::Free();
}

