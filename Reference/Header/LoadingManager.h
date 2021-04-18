#pragma once

#include "Base.h"

BEGIN(Engine)
class CComponent_Manager;

class ENGINE_DLL CLoadingManager: public CBase{
	// ΩÃ±€≈Ê ¡§¿«
	DECLARE_SINGLETON(CLoadingManager)

	typedef HRESULT(*LoadFuncType)(void);

private:
	explicit CLoadingManager();
	virtual ~CLoadingManager() = default;
public:
	_uint IsComplete() const{
		return m_iComplete;
	}
public:
	HRESULT InitLoadManager(LoadFuncType _LoadFunc);
public:
	static _uint APIENTRY ThreadMain(void* pArg);
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	LoadFuncType			m_LoadingFunc = nullptr;

	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CS;
	_uint					m_iComplete;

public:
	virtual void Free();
};

END