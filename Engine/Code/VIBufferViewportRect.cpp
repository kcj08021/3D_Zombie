#include "..\Header\VIBufferViewportRect.h"

CVIBufferViewportRect::CVIBufferViewportRect(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CVIBuffer(_pGraphicDevice){
}

CVIBufferViewportRect::CVIBufferViewportRect(const CVIBufferViewportRect & _VIBufferViewportRect)
	: CVIBuffer(_VIBufferViewportRect){
}

HRESULT CVIBufferViewportRect::InitComponent_Proto(_float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY){
	m_uiStride = sizeof(_VERTEX_VIEWPORT_TEX);
	m_uiVerticesCount = 4;
	m_ulFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

	m_uiPolygonIndicesSize = sizeof(INDEX16);
	m_eFormat = D3DFMT_INDEX16;
	m_uiPrimitiveCount = 2;

	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	_VERTEX_VIEWPORT_TEX* pVertices = nullptr;

	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPos = _vec4(_fStartX, _fStartY, 0.f, 1.f);
	pVertices[0].vTextureCoord = _vec2(0.f, 0.f);

	pVertices[1].vPos = _vec4(_fStartX + _fSizeX, _fStartY, 0.f, 1.f);
	pVertices[1].vTextureCoord = _vec2(1.f, 0.f);

	pVertices[2].vPos = _vec4(_fStartX + _fSizeX, _fStartY + _fSizeY, 0.f, 1.f);
	pVertices[2].vTextureCoord = _vec2(1.f, 1.f);

	pVertices[3].vPos = _vec4(_fStartX, _fStartY + _fSizeY, 0.f, 1.f);
	pVertices[3].vTextureCoord = _vec2(0.f, 1.f);

	m_pVertexBuffer->Unlock();

	INDEX16* pIndices = nullptr;

	m_pIndexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	pIndices[0]._1 = 0;
	pIndices[0]._2 = 1;
	pIndices[0]._3 = 2;

	pIndices[1]._1 = 0;
	pIndices[1]._2 = 2;
	pIndices[1]._3 = 3;

	m_pIndexBuffer->Unlock();
	
	return NOERROR;
}

HRESULT CVIBufferViewportRect::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

HRESULT CVIBufferViewportRect::RenderVIBuffer(){
	LPDIRECT3DDEVICE9 pGraphicDevice = CComponent::GetGraphicDevice();
	if(nullptr == pGraphicDevice
		|| nullptr == m_pVertexBuffer
		|| nullptr == m_pIndexBuffer)
		return E_FAIL;

	SafeAddRef(pGraphicDevice);

	pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_uiStride);
	pGraphicDevice->SetFVF(m_ulFVF);
	pGraphicDevice->SetIndices(m_pIndexBuffer);
	pGraphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uiVerticesCount, 0, m_uiPrimitiveCount);

	SafeRelease(pGraphicDevice);

	return NOERROR;
}

CVIBufferViewportRect * CVIBufferViewportRect::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY){
	CVIBufferViewportRect* pInstance = new CVIBufferViewportRect(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_fStartX, _fStartY, _fSizeX, _fSizeY))){
		SafeRelease(pInstance);
		MSG_BOX(L"CVIBufferViewportRect Create Failure");
	}
	
	return pInstance;
}

CComponent * CVIBufferViewportRect::CreateClone(void * _pArgument){
	CVIBufferViewportRect* pInstance = new CVIBufferViewportRect(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CVIBufferViewportRect Clone Failure");
	}

	return pInstance;
}

void CVIBufferViewportRect::Free(){
	CVIBuffer::Free();
}
