#include "stdafx.h"
// Client
#include "..\Header\AmmoHUD.h"
#include "..\Header\UnitManager.h"
#include "..\Header\Weapon.h"

USING(Client)

CAmmoHUD::CAmmoHUD(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CTextUI(_pGraphicDevice){
}

CAmmoHUD::CAmmoHUD(const CAmmoHUD & _AmmoHUD)
	: CTextUI(_AmmoHUD){
}

HRESULT CAmmoHUD::InitGameObject_Proto(D3DXFONT_DESC* _pDesc){
	return CTextUI::InitGameObject_Proto(_pDesc);
}

HRESULT CAmmoHUD::InitGameObject_Clone(void * _pArgument){
	return CTextUI::InitGameObject_Clone(_pArgument);
}

HRESULT CAmmoHUD::LateInitGameObject(){
	GetWeaponAmmo();

	return NOERROR;
}

HRESULT CAmmoHUD::UpdateGameObject(_double _dDeltaTime){

	return NOERROR;
}

HRESULT CAmmoHUD::LateUpdateGameObject(_double _dDeltaTime){
	GetWeaponAmmo();
	return CTextUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CAmmoHUD::RenderGameObject(_double _dDeltaTime){

	return CTextUI::RenderGameObject(_dDeltaTime);
}

void CAmmoHUD::GetWeaponAmmo(){
	const CWeapon* pWeapon = CUnitManager::GetInstance()->GetPlayerWeapon();
	if(nullptr != pWeapon){
		int iCurrentMagazine = pWeapon->GetCurrentMagazine();
		int iCurrentAmmo = pWeapon->GetCurrentAmmo();

		TCHAR szMagazine[64] = {};
		TCHAR szAmmo[64] = {};

		_itot_s(iCurrentMagazine, szMagazine, 10);
		_itot_s(iCurrentAmmo, szAmmo, 10);

		lstrcpy(m_pString, szMagazine);
		lstrcat(m_pString, L"/");
		lstrcat(m_pString, szAmmo);
	}
}

CAmmoHUD * CAmmoHUD::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc){
	CAmmoHUD* pInstance = new CAmmoHUD(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pDesc))){
		MSG_BOX(L"CAmmoHUD Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CAmmoHUD::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CAmmoHUD(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CAmmoHUD Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CAmmoHUD::Free(){
	CTextUI::Free();
}
