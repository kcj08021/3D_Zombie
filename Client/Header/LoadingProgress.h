#pragma once
#include "ImageUI.h"

BEGIN(Client)

class CLoadingProgress:
	public CImageUI{
private:
	explicit CLoadingProgress(LPDIRECT3DDEVICE9 m_pGraphicDevice);
	explicit CLoadingProgress(const CLoadingProgress& _LoadingProgress);
	virtual ~CLoadingProgress() = default;

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
	void SetLoadingProgress(_float _fLoadingProgress){
		m_fLoadingProgress = _fLoadingProgress;
	}

private:
	HRESULT SettingShader();

private:
	_float m_fLoadingProgress = 0.f;

public:
	static CLoadingProgress* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// CImageUI을(를) 통해 상속됨
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END