#ifndef __VI_BUFFER_CUBE_H__
#define __VI_BUFFER_CUBE_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBufferCube final
	: public CVIBuffer{
private:
	// ������
	explicit CVIBufferCube(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CVIBufferCube(const CVIBufferCube& _VIBufferCube);
	// �Ҹ���
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
	// ���� ���� �Լ�(static)
	static CVIBufferCube* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !
