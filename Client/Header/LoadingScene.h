#pragma once
#include "Scene.h"
BEGIN(Client)
class CLoadingProgress;
class CLoadingScene:
	public CScene{
private:
	// ������
	explicit CLoadingScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// �Ҹ���
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


	// ������ ���� ����
private:
	HANDLE m_hThread;
	CRITICAL_SECTION m_CS;
	_uint m_iComplete;

private:
	CLoadingProgress* m_pLoadingProgress = nullptr;
	_uint m_uiLoadProgress = 0;
	_float m_fLoadProgress = 0;

public:
	// ���� �Լ�(static)
	static CLoadingScene* Create(LPDIRECT3DDEVICE9 _pGraphicDevice);

public:
	// Free
	virtual void Free();

};

END