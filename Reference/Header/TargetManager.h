#ifndef __TARGET_MANAGER_H__
#define __TARGET_MANAGER_H__

#include "Base.h"
#include "Target.h"

BEGIN(Engine)

class CTargetManager:
	public CBase{
	DECLARE_SINGLETON(CTargetManager)

private:
	explicit CTargetManager();
	virtual ~CTargetManager() = default;

public:
	HRESULT AddRenderTarget(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szTargetKey, _uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor);
	HRESULT SettingMultiRenderTarget(const _tchar* _szMRTKey, const _tchar* _szTargetKey);
	HRESULT SettingDebugBuffer(const _tchar* _szKey, _float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY);
	HRESULT SettingOnShader(CShader* _pShader, const char* _szConstantTable, const _tchar* _szTargetKey);

public:
	HRESULT BeginMultiRenderTarget(const _tchar* _szMRTKey);
	HRESULT EndMultiRenderTarget(const _tchar* _szMRTKey);
	HRESULT RenderDebugBuffer(const _tchar* _szMRTKey);

private:
	// 렌더타겟을 보관하는 컨테이너
	map<const _tchar*, CTarget*> m_RenderTargetMap;
	typedef map<const _tchar*, CTarget*> RENDERTARGET_MAP;
	// 렌더타겟들을 묶어서 보관하는 컨테이너
	map<const _tchar*, list<CTarget*>> m_MultiRenderTargetMap;
	typedef map<const _tchar*, list<CTarget*>> MULTITARGET_MAP;

private:
	CTarget* FindeRenderTarget(const _tchar* _szKey);
	list<CTarget*>* FindMultiRenderTargat(const _tchar* _szKey);

public:
	virtual void Free();
};

END

#endif // !__TARGET_MANAGER_H__
