#ifndef __VI_BUFFER_RECT_H__
#define __VI_BUFFER_RECT_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferRect final: public CVIBuffer{
private:
	// 생성자
	explicit CVIBufferRect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CVIBufferRect(const CVIBufferRect& _VIBufferRect);
	// 소멸자
	virtual ~CVIBufferRect() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;
public:
	// Render
	virtual HRESULT RenderVIBufferRect();

public:
	// 원본 생성 함수(static)
	static CVIBufferRect* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__VI_BUFFER_RECT_H__
