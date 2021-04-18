#pragma once
#include "Scene.h"
BEGIN(Client)
class CLoadingProgress;
class CLoadingScene:
	public CScene{
private:
	// 생성자
	explicit CLoadingScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
	virtual ~CLoadingScene() = default;

private:
	// Initialize
	virtual HRESULT InitScene();
public:
	// Update
	virtual HRESULT UpdateScene(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateScene(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderScene();

public:
	static _uint APIENTRY ThreadMain(void* pArg);

private:
	HRESULT LoadingObject();


	// 스레드 관련 변수
private:
	HANDLE m_hThread;
	CRITICAL_SECTION m_CS;
	_uint m_iComplete;

private:
	CLoadingProgress* m_pLoadingProgress = nullptr;
	_uint m_uiLoadProgress = 0;
	_float m_fLoadProgress = 0;

public:
	// 생성 함수(static)
	static CLoadingScene* Create(LPDIRECT3DDEVICE9 _pGraphicDevice);

public:
	// Free
	virtual void Free();

};

END