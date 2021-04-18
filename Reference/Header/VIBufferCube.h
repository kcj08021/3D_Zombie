#ifndef __VI_BUFFER_CUBE_H__
#define __VI_BUFFER_CUBE_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferCube final
	: public CVIBuffer{
private:
	// 생성자
	explicit CVIBufferCube(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CVIBufferCube(const CVIBufferCube& _VIBufferCube);
	// 소멸자
	virtual ~CVIBufferCube() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;
public:
	// Render
	virtual HRESULT RenderVIBufferCube();

public:
	// 원본 생성 함수(static)
	static CVIBufferCube* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !
