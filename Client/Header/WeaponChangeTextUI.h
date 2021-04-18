#ifndef __WEAPON_CHANGE_TEXT_UI_H__
#define __WEAPON_CHANGE_TEXT_UI_H__

#include "TextUI.h"


BEGIN(Client)
class CWeaponChangeTextUI:
	public CTextUI{
public:
	explicit CWeaponChangeTextUI(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CWeaponChangeTextUI(const CWeaponChangeTextUI& _WeaponChangeTextUI);
	virtual ~CWeaponChangeTextUI() = default;

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
	void SettingUIEnable();

	// CTextUI을(를) 통해 상속됨
public:
	static CWeaponChangeTextUI* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc);
	virtual CGameObject * CreateClone(void * _pArgument) override;

	virtual void Free();
};
END

#endif // !__WEAPON_CHANGE_TEXT_UI_H__
