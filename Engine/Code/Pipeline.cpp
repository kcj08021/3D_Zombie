#include "..\Header\Pipeline.h"

USING(Engine)
// �̱��� ����
IMPLEMENT_SINGLETON(CPipeline)

CPipeline::CPipeline(){
	::D3DXMatrixIdentity(&m_tViewMatrix);
	::D3DXMatrixIdentity(&m_tProjectionMatrix);
	::D3DXMatrixIdentity(&m_matCameraWorldMatrix);
}

// ��ȯ ����� ������ �Լ�
HRESULT CPipeline::GetTransform(D3DTRANSFORMSTATETYPE _eTransform,  _matrix* _pMatrix){
	if(nullptr == _pMatrix)
		return E_FAIL;

	switch(_eTransform){
	case D3DTS_VIEW:
		*_pMatrix = m_tViewMatrix;
		break;
	case D3DTS_PROJECTION:
		*_pMatrix = m_tProjectionMatrix;
		break;
	default:
		break;
	}

	return NOERROR;
}

// ��ȯ ����� ���� �ϴ� �Լ�
HRESULT CPipeline::SetTransform(D3DTRANSFORMSTATETYPE _eTransform, const _matrix & _tMatirx){
	switch(_eTransform){
	case D3DTS_VIEW:
		m_tViewMatrix = _tMatirx;
		break;
	case D3DTS_PROJECTION:
		m_tProjectionMatrix = _tMatirx;
		break;
	default:
		return E_FAIL;
		break;
	}
	
	return NOERROR;
}

// Free
void CPipeline::Free(){

}