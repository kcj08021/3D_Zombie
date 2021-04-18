#ifndef __AMMO_HUD_H__
#define __AMMO_HUD_H__

#include "TextUI.h"

BEGIN(Client)
class CAmmoHUD:
	public CTextUI{
public:
	explicit CAmmoHUD(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CAmmoHUD(const CAmmoHUD& _AmmoHUD);
	virtual ~CAmmoHUD() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(D3DXFONT_DESC* _pDesc);
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

private:
	void GetWeaponAmmo();

public:
	// 원본 생성 함수(static)
	static CAmmoHUD* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};
END

#endif // !__AMMO_HUD_H__
