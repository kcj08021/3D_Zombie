#include "..\Header\VIBufferRect.h"

USING(Engine)

// 생성자
CVIBufferRect::CVIBufferRect(LPDIRECT3DDEVICE9 _pGraphicDevice):
	CVIBuffer(_pGraphicDevice){
}
	

// 복사 생성자
CVIBufferRect::CVIBufferRect(const CVIBufferRect & _VIBufferRect):
	CVIBuffer(_VIBufferRect){
}

// Initialize with Prototype
HRESULT CVIBufferRect::InitComponent_Proto(){
	// 버텍스 구조체의 사이즈 설정
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// 버텍스의 갯수 설정
	m_uiVerticesCount = 4;
	// FVF 설정
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	// 버텍스로 그려질 삼각형의 갯수 설정
	m_uiPrimitiveCount = 2;
	// 인덱스 버퍼 원소 3개의 사이즈
	m_uiPolygonIndicesSize = sizeof(INDEX16);
	// 인덱스 버퍼의 포맷 설정
	m_eFormat = D3DFMT_INDEX16;

	// 설정한 정보로 버텍스 버퍼와 인덱스 버퍼 할당
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;
	
	////////////////////////////////////////////////////////////////////////
	// 버텍스에 좌표 입력

	// 버텍스 버퍼의 주소를 받아올 포인터 변수
	_VERTEX_TEX_NORMAL* pVertex = nullptr;

	// 버텍스 버퍼의 공간을 잠금
	m_pVertexBuffer->Lock(0,0,(void**)&pVertex, 0);

	// 해당 공간에 값(버텍스의 정보)을 채운다.
	// 좌측 상단
	pVertex[0].vPos = _vec3(-0.5f, 0.5f, 0.0f);
	pVertex[0].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[0].vTextureCoord = { 0.f,0.f };

	// 우측 상단
	pVertex[1].vPos = _vec3(0.5f, 0.5f, 0.0f);
	pVertex[1].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[1].vTextureCoord = { 1.f,0.f };

	// 우측 하단
	pVertex[2].vPos = _vec3(0.5f, -0.5f, 0.0f);
	pVertex[2].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[2].vTextureCoord = { 1.f,1.f };

	// 좌측 하단
	pVertex[3].vPos = _vec3(-0.5f, -0.5f, 0.0f);
	pVertex[3].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[3].vTextureCoord = { 0.f,1.f };

	// 버퍼 공간의 잠금을 해제
	m_pVertexBuffer->Unlock();

	////////////////////////////////////////////////////////////////////////
	// 인덱스 버퍼에 버텍스 버퍼 인덱스 입력

	// 인덱스 버퍼에 들어있는 버텍스 버퍼의 인덱스 번호를 받을 구조체 포인터
	INDEX16* pIndices = nullptr;

	// 인덱스 버퍼 공간의 잠금(버텍스 버퍼와 인자 같음)
	m_pIndexBuffer->Lock(0,0, reinterpret_cast<void**>(&pIndices),0);

	// 구조체의 형태로 받아서 구조체의 멤버로 접근이 가능함
	// 구조체 하나에 삼각형 하나라고 생각하면 편함

	// 첫번째 삼각형
	pIndices[0]._1 = 0;		//버텍스 버퍼의 0번 인덱스
	pIndices[0]._2 = 1;		//버텍스 버퍼의 1번 인덱스
	pIndices[0]._3 = 2;		//버텍스 버퍼의 2번 인덱스

	//두번째 삼각형
	pIndices[1]._1 = 0;		//버텍스 버퍼의 0번 인덱스
	pIndices[1]._2 = 2;		//버텍스 버퍼의 2번 인덱스
	pIndices[1]._3 = 3;		//버텍스 버퍼의 3번 인덱스

	// 버퍼 공간의 잠금을 해제
	m_pIndexBuffer->Unlock();

	return NOERROR;
}

// Initialize with Clone
HRESULT CVIBufferRect::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

HRESULT CVIBufferRect::RenderVIBufferRect(){
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

// 프로토타입 생성 함수
CVIBufferRect * CVIBufferRect::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CVIBufferRect * pInstance = new CVIBufferRect(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		MSG_BOX(L"CVIBufferRect Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// 복제 생성 함수
CComponent * CVIBufferRect::CreateClone(void* _pArgument){
	CVIBufferRect * pInstance = new CVIBufferRect(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CVIBufferRect Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// Free
void CVIBufferRect::Free(){
	CVIBuffer::Free();
}
