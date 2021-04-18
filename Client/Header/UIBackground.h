#pragma once

#include "ImageUI.h"

BEGIN(Client)

class CUIBackground final : public CImageUI{
private:
	explicit CUIBackground(LPDIRECT3DDEVICE9 m_pGraphicDevice);
	explicit CUIBackground(const CUIBackground& _UIBackground);
	virtual ~CUIBackground() = default;

protected:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
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
	static CUIBackground* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// CImageUI을(를) 통해 상속됨
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END