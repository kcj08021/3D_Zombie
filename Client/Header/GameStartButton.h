#pragma once
#include "TextUI.h"

BEGIN(Client)
class CGameStartButton final:
	public CTextUI{
private:
	explicit CGameStartButton(LPDIRECT3DDEVICE9 m_pGraphicDevice);
	explicit CGameStartButton(const CGameStartButton& _UIBackground);
	virtual ~CGameStartButton() = default;

protected:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(D3DXFONT_DESC * _pDesc);
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
	static CGameStartButton* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC * _pDesc);
	// CImageUI을(를) 통해 상속됨
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END
