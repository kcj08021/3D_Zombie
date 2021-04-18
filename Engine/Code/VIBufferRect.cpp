#include "..\Header\VIBufferRect.h"

USING(Engine)

// ������
CVIBufferRect::CVIBufferRect(LPDIRECT3DDEVICE9 _pGraphicDevice):
	CVIBuffer(_pGraphicDevice){
}
	

// ���� ������
CVIBufferRect::CVIBufferRect(const CVIBufferRect & _VIBufferRect):
	CVIBuffer(_VIBufferRect){
}

// Initialize with Prototype
HRESULT CVIBufferRect::InitComponent_Proto(){
	// ���ؽ� ����ü�� ������ ����
	m_uiStride = sizeof(_VERTEX_TEX_NORMAL);
	// ���ؽ��� ���� ����
	m_uiVerticesCount = 4;
	// FVF ����
	m_ulFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	// ���ؽ��� �׷��� �ﰢ���� ���� ����
	m_uiPrimitiveCount = 2;
	// �ε��� ���� ���� 3���� ������
	m_uiPolygonIndicesSize = sizeof(INDEX16);
	// �ε��� ������ ���� ����
	m_eFormat = D3DFMT_INDEX16;

	// ������ ������ ���ؽ� ���ۿ� �ε��� ���� �Ҵ�
	if(FAILED(CVIBuffer::InitComponent_Proto()))
		return E_FAIL;
	
	////////////////////////////////////////////////////////////////////////
	// ���ؽ��� ��ǥ �Է�

	// ���ؽ� ������ �ּҸ� �޾ƿ� ������ ����
	_VERTEX_TEX_NORMAL* pVertex = nullptr;

	// ���ؽ� ������ ������ ���
	m_pVertexBuffer->Lock(0,0,(void**)&pVertex, 0);

	// �ش� ������ ��(���ؽ��� ����)�� ä���.
	// ���� ���
	pVertex[0].vPos = _vec3(-0.5f, 0.5f, 0.0f);
	pVertex[0].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[0].vTextureCoord = { 0.f,0.f };

	// ���� ���
	pVertex[1].vPos = _vec3(0.5f, 0.5f, 0.0f);
	pVertex[1].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[1].vTextureCoord = { 1.f,0.f };

	// ���� �ϴ�
	pVertex[2].vPos = _vec3(0.5f, -0.5f, 0.0f);
	pVertex[2].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[2].vTextureCoord = { 1.f,1.f };

	// ���� �ϴ�
	pVertex[3].vPos = _vec3(-0.5f, -0.5f, 0.0f);
	pVertex[3].vNormal = _vec3(0.f, 0.f, 0.f);
	pVertex[3].vTextureCoord = { 0.f,1.f };

	// ���� ������ ����� ����
	m_pVertexBuffer->Unlock();

	////////////////////////////////////////////////////////////////////////
	// �ε��� ���ۿ� ���ؽ� ���� �ε��� �Է�

	// �ε��� ���ۿ� ����ִ� ���ؽ� ������ �ε��� ��ȣ�� ���� ����ü ������
	INDEX16* pIndices = nullptr;

	// �ε��� ���� ������ ���(���ؽ� ���ۿ� ���� ����)
	m_pIndexBuffer->Lock(0,0, reinterpret_cast<void**>(&pIndices),0);

	// ����ü�� ���·� �޾Ƽ� ����ü�� ����� ������ ������
	// ����ü �ϳ��� �ﰢ�� �ϳ���� �����ϸ� ����

	// ù��° �ﰢ��
	pIndices[0]._1 = 0;		//���ؽ� ������ 0�� �ε���
	pIndices[0]._2 = 1;		//���ؽ� ������ 1�� �ε���
	pIndices[0]._3 = 2;		//���ؽ� ������ 2�� �ε���

	//�ι�° �ﰢ��
	pIndices[1]._1 = 0;		//���ؽ� ������ 0�� �ε���
	pIndices[1]._2 = 2;		//���ؽ� ������ 2�� �ε���
	pIndices[1]._3 = 3;		//���ؽ� ������ 3�� �ε���

	// ���� ������ ����� ����
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

	// ���ؽ� ���� ��� �غ�
	pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_uiStride);
	// FVF ����
	pGraphicDevice->SetFVF(m_ulFVF);
	// �ε��� ���� ����
	pGraphicDevice->SetIndices(m_pIndexBuffer);
	// �ε��� ���۸� ���� ���ؽ� �����Ͽ� ���
	pGraphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_uiVerticesCount, 0, m_uiPrimitiveCount);
	
	SafeRelease(pGraphicDevice);

	return NOERROR;
}

// ������Ÿ�� ���� �Լ�
CVIBufferRect * CVIBufferRect::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CVIBufferRect * pInstance = new CVIBufferRect(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		MSG_BOX(L"CVIBufferRect Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// ���� ���� �Լ�
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
