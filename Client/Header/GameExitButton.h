#pragma once
#include "TextUI.h"

BEGIN(Engine)
class CPickee;
class CVIBufferRect;
END

BEGIN(Client)

class CGameExitButton:
	public CTextUI{
private:
	explicit CGameExitButton(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CGameExitButton(const CGameExitButton& _AmmoHUD);
	virtual ~CGameExitButton() = default;

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

public:
	_bool IsClick(){
		return m_bIsClick;
	}

private:
	_bool m_bIsClick = false;
	_bool m_bSound = false;

public:
	// 원본 생성 함수(static)
	static CGameExitButton* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC* _pDesc);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};

END
