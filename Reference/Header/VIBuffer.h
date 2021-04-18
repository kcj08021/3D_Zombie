#ifndef __VI_BUFFER_H__
#define __VI_BUFFER_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract:
	public CComponent{
protected:
	// 생성자
	explicit CVIBuffer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CVIBuffer(const CVIBuffer& _VIBuffer);
	// 소멸자
	virtual ~CVIBuffer() = default;

public:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;

public:
	// 피킹 검사를 하기 위한 배열들을 받아오는 함수
	HRESULT GetPickingCheckInfo(const POLYGON_POSITION*& _pPolygonPosition, _uint& _uiPrimitiveCount) const;

protected:
	// 버텍스 구조체의 크기
	_uint m_uiStride = 0;
	// 버텍스의 갯수
	_uint m_uiVerticesCount = 0;
	// 버텍스 정보(FVF, Flexible Vertex Format)
	_ulong m_ulFVF = 0;
	// 그려야할 도형(삼각형)의 갯수
	_uint m_uiPrimitiveCount = 0;
	// 인덱스 버퍼 원소 3개의 크기(Byte 단위)
	_uint m_uiPolygonIndicesSize = 0;
	// 인덱스 버퍼의 버퍼 포맷
	D3DFORMAT m_eFormat;
	// 버텍스의 위치들을 저장할 구조체 포인터
	POLYGON_POSITION* m_pPolygonPositionArray = nullptr;

	// 버텍스 버퍼 변수
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = nullptr;
	// 인덱스 버퍼 변수
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer = nullptr;
	
public:
	// 복제 생성 함수
	virtual CComponent * CreateClone(void* _pArgument) override PURE;
	// Free
	virtual void Free();
};
END

#endif // !__VI_BUFFER_H__
