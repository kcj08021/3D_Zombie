#ifndef __HP_UI_H__
#define __HP_UI_H__

#include "TextUI.h"

BEGIN(Client)
class CHPUI:
	public CTextUI{
public:
	explicit CHPUI(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CHPUI(const CHPUI& _HPUI);
	virtual ~CHPUI() = default;

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
	void GetPlayerHP();

public:
	// 원본 생성 함수(static)
	static CHPUI* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};
END

#endif // !__HP_UI_H__
