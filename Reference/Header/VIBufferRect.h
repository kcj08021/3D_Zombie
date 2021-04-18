#ifndef __VI_BUFFER_RECT_H__
#define __VI_BUFFER_RECT_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferRect final: public CVIBuffer{
private:
	// ������
	explicit CVIBufferRect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CVIBufferRect(const CVIBufferRect& _VIBufferRect);
	// �Ҹ���
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
	// ���� ���� �Լ�(static)
	static CVIBufferRect* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__VI_BUFFER_RECT_H__
