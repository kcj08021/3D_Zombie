#include "..\Header\VIBuffer.h"

// 생성자
CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_uiStride(0)
	, m_uiVerticesCount(0)
	, m_ulFVF(0)
	, m_uiPrimitiveCount(0)
	, m_uiPolygonIndicesSize(0)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_pPolygonPositionArray(nullptr){
}

// 복사 생성자
CVIBuffer::CVIBuffer(const CVIBuffer & _VIBuffer)
	: CComponent(_VIBuffer)
	, m_uiStride(_VIBuffer.m_uiStride)
	, m_uiVerticesCount(_VIBuffer.m_uiVerticesCount)
	, m_ulFVF(_VIBuffer.m_ulFVF)
	, m_uiPrimitiveCount(_VIBuffer.m_uiPrimitiveCount)
	, m_uiPolygonIndicesSize(_VIBuffer.m_uiPolygonIndicesSize)
	, m_eFormat(_VIBuffer.m_eFormat)
	, m_pVertexBuffer(_VIBuffer.m_pVertexBuffer)
	, m_pIndexBuffer(_VIBuffer.m_pIndexBuffer)
	, m_pPolygonPositionArray(_VIBuffer.m_pPolygonPositionArray){
	SafeAddRef(_VIBuffer.m_pVertexBuffer);
	SafeAddRef(_VIBuffer.m_pIndexBuffer);
}

// Initialize with Prototype
HRESULT CVIBuffer::InitComponent_Proto(){
	LPDIRECT3DDEVICE9 pGraphicDevice = CComponent::GetGraphicDevice();
	if(nullptr == pGraphicDevice)
		return E_FAIL;

	SafeAddRef(pGraphicDevice);

	HRESULT hr = 0;

	// 버텍스 버퍼 생성
	hr = pGraphicDevice->CreateVertexBuffer(m_uiStride * m_uiVerticesCount, 0, m_ulFVF, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr);
	
	// 인덱스 버퍼 생성
	hr = pGraphicDevice->CreateIndexBuffer(m_uiPolygonIndicesSize * m_uiPrimitiveCount, 0, m_eFormat, D3DPOOL_MANAGED, &m_pIndexBuffer, nullptr);

	SafeRelease(pGraphicDevice);

	return hr;
}

// Initialize with Clone
HRESULT CVIBuffer::InitComponent_Clone(void* _pArgument){

	return NOERROR;
}

// 피킹 검사를 하기 위한 배열들을 받아오는 함수
HRESULT CVIBuffer::GetPickingCheckInfo(const POLYGON_POSITION *& _pPolygonPositionArray, _uint& _uiPrimitiveCount) const{
	if(nullptr == m_pPolygonPositionArray)
		return E_FAIL;
	
	_pPolygonPositionArray = m_pPolygonPositionArray;
	_uiPrimitiveCount = m_uiPrimitiveCount;
	
	return NOERROR;
}

// Free
void CVIBuffer::Free(){
	if(!CComponent::IsClone())
		SafeDelete_Array(m_pPolygonPositionArray);

	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);

	CComponent::Free();
}
