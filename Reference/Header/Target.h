#ifndef __TARGET_H__
#define __TARGET_H__

#include "Base.h"

BEGIN(Engine)
class CShader;
class CVIBufferViewportRect;

class CTarget:
	public CBase{
private:
	explicit CTarget(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual ~CTarget() = default;

private:
	HRESULT InitTarget(_uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor);

public:
	HRESULT ClearRenderTarget();
	HRESULT BeginRenderTargat(_uint _uiIndex);
	HRESULT EndRenderTargat(_uint _uiIndex);

public:
	HRESULT SettingDebugBuffer(_float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY);
	HRESULT RenderDebugBuffer();
	HRESULT SettingOnShader(CShader * _pShader, const char * _szConstantTable);

private:
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pSurface = nullptr;
	LPDIRECT3DSURFACE9 m_pOldSurface = nullptr;
	CVIBufferViewportRect* m_pVIBufferCom = nullptr;
	D3DXCOLOR m_ClearColor;

public:
	static CTarget* CreateTarget(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor);

public:
	virtual void Free();
};
END

#endif // !__TARGET_H__ 