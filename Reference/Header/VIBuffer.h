#ifndef __VI_BUFFER_H__
#define __VI_BUFFER_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract:
	public CComponent{
protected:
	// ������
	explicit CVIBuffer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CVIBuffer(const CVIBuffer& _VIBuffer);
	// �Ҹ���
	virtual ~CVIBuffer() = default;

public:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;

public:
	// ��ŷ �˻縦 �ϱ� ���� �迭���� �޾ƿ��� �Լ�
	HRESULT GetPickingCheckInfo(const POLYGON_POSITION*& _pPolygonPosition, _uint& _uiPrimitiveCount) const;

protected:
	// ���ؽ� ����ü�� ũ��
	_uint m_uiStride = 0;
	// ���ؽ��� ����
	_uint m_uiVerticesCount = 0;
	// ���ؽ� ����(FVF, Flexible Vertex Format)
	_ulong m_ulFVF = 0;
	// �׷����� ����(�ﰢ��)�� ����
	_uint m_uiPrimitiveCount = 0;
	// �ε��� ���� ���� 3���� ũ��(Byte ����)
	_uint m_uiPolygonIndicesSize = 0;
	// �ε��� ������ ���� ����
	D3DFORMAT m_eFormat;
	// ���ؽ��� ��ġ���� ������ ����ü ������
	POLYGON_POSITION* m_pPolygonPositionArray = nullptr;

	// ���ؽ� ���� ����
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = nullptr;
	// �ε��� ���� ����
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer = nullptr;
	
public:
	// ���� ���� �Լ�
	virtual CComponent * CreateClone(void* _pArgument) override PURE;
	// Free
	virtual void Free();
};
END

#endif // !__VI_BUFFER_H__
