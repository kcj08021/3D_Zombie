#include "..\Header\VIBufferCube.h"

CVIBufferCube::CVIBufferCube(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CVIBuffer(_pGraphicDevice)
	{
}

CVIBufferCube::CVIBufferCube(const CVIBufferCube & _VIBufferCube)
	: CVIBuffer(_VIBufferCube){
}

HRESULT CVIBufferCube::InitComponent_Proto(){
	// 버텍스 구조체의 사이즈 설정
	m_uiStride = sizeof(_VERTEX_TEX_CUBE);
	// 버텍스의 갯수 설정
	m_uiVerticesCount = 8;
	// FVF 설정
	m_ulFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	// 버텍스로 그려질 삼각형의 갯수 설정
	m_uiPrimitiveCount = 12;
	// 인덱스 버퍼 원소 3개의 사이즈
	m_uiPolygonIndicesSize = sizeof(INDEX16);
	// 인덱스 버퍼의 포맷 설정
	m_eFormat = D3DFMT_INDEX16;

	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;

	_VERTEX_TEX_CUBE*		pVertices = nullptr;

	// 공간을 걸어잠근다.
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

	// 해당 공간에 값을 채운다.
	pVertices[0].vPos = _vec3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTextureCoord = pVertices[0].vPos;

	pVertices[1].vPos = _vec3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTextureCoord = pVertices[1].vPos;

	pVertices[2].vPos = _vec3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTextureCoord = pVertices[2].vPos;

	pVertices[3].vPos = _vec3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTextureCoord = pVertices[3].vPos;

	pVertices[4].vPos = _vec3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTextureCoord = pVertices[4].vPos;

	pVertices[5].vPos = _vec3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTextureCoord = pVertices[5].vPos;

	pVertices[6].vPos = _vec3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTextureCoord = pVertices[6].vPos;

	pVertices[7].vPos = _vec3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTextureCoord = pVertices[7].vPos;

	m_pVertexBuffer->Unlock();

	INDEX16*		pIndices = nullptr;

	m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);

	// +x
	pIndices[0]._1 = 1; pIndices[0]._2 = 5; pIndices[0]._3 = 6;
	pIndices[1]._1 = 1; pIndices[1]._2 = 6; pIndices[1]._3 = 2;

	// -x
	pIndices[2]._1 = 4; pIndices[2]._2 = 0; pIndices[2]._3 = 3;
	pIndices[3]._1 = 4; pIndices[3]._2 = 3; pIndices[3]._3 = 7;

	// +y
	pIndices[4]._1 = 4; pIndices[4]._2 = 5; pIndices[4]._3 = 1;
	pIndices[5]._1 = 4; pIndices[5]._2 = 1; pIndices[5]._3 = 0;

	// -y
	pIndices[6]._1 = 3; pIndices[6]._2 = 2; pIndices[6]._3 = 6;
	pIndices[7]._1 = 3; pIndices[7]._2 = 6; pIndices[7]._3 = 7;

	// +z						 					 	
	pIndices[8]._1 = 5; pIndices[8]._2 = 4; pIndices[8]._3 = 7;
	pIndices[9]._1 = 5; pIndices[9]._2 = 7; pIndices[9]._3 = 6;

	// -z		
	pIndices[10]._1 = 0; pIndices[10]._2 = 1; pIndices[10]._3 = 2;
	pIndices[11]._1 = 0; pIndices[11]._2 = 2; pIndices[11]._3 = 3;

	m_pIndexBuffer->Unlock();

	return NOERROR;
}

HRESULT CVIBufferCube::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

HRESULT CVIBufferCube::RenderVIBufferCube(){
	LPDIRECT3DDEVICE9 pGraphicDevice = GetGraphicDevice();
	if(nullptr == pGraphicDevice
		|| nullptr == m_pVertexBuffer
		|| nullptr == m_pIndexBuffer)
		return E_FAIL;

	SafeAddRef(pGraphicDevice);

	// 버텍스 버퍼 출력 준비
	pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_uiStride);
	// FVF 설정
	pGraphicDevice->SetFVF(m_ulFVF);
	// 인덱스 버퍼 설정
	pGraphicDevice->SetIndices(m_pIndexBuffer);
	// 인덱스 버퍼를 통해 버텍스 참조하여 출력
	pGraphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uiVerticesCount, 0, m_uiPrimitiveCount);

	SafeRelease(pGraphicDevice);

	return NOERROR;
}

CVIBufferCube * CVIBufferCube::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CVIBufferCube* pInstance = new CVIBufferCube(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CVIBufferCube Create Failure");
	}

	return pInstance;
}

CComponent * CVIBufferCube::CreateClone(void * _pArgument){
	CVIBufferCube* pInstance = new CVIBufferCube(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CVIBufferCube Clone Failure");
	}

	return pInstance;
}

void CVIBufferCube::Free(){
	CVIBuffer::Free();
}
