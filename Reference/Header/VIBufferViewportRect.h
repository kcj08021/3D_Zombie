#ifndef __VI_BUFFER_VIEWPORT_RECT_H__
#define __VI_BUFFER_VIEWPORT_RECT_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferViewportRect final:
	public CVIBuffer{
private:
	explicit CVIBufferViewportRect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CVIBufferViewportRect(const CVIBufferViewportRect& _VIBufferViewportRect);
	virtual ~CVIBufferViewportRect() = default;

private:
	HRESULT InitComponent_Proto(_float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY);
	HRESULT InitComponent_Clone(void * _pArgument);

public:
	HRESULT RenderVIBuffer();

public:
	static CVIBufferViewportRect* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};
END

#endif // !__VI_BUFFER_VIEWPORT_RECT_H__
