#include "..\Header\Camera.h"

// ������
CCamera::CCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pTransformCom(nullptr)
	, m_pPipeline(CPipeline::GetInstance()){
	SafeAddRef(m_pPipeline);
	::D3DXMatrixIdentity(&m_tViewMatrix);
	::D3DXMatrixIdentity(&m_tProjectionMatrix);
}

// ���� ������
CCamera::CCamera(const CCamera & _Camera)
	: CGameObject(_Camera)
	, m_pTransformCom(_Camera.m_pTransformCom)
	, m_pPipeline(_Camera.m_pPipeline)
	, m_tViewMatrix(_Camera.m_tViewMatrix)
	, m_tProjectionMatrix(_Camera.m_tProjectionMatrix){
	SafeAddRef(_Camera.m_pPipeline);
}

// Initialize with Prototype
HRESULT CCamera::InitGameObject_Proto(){
	return NOERROR;
}

// Initialize with Clone
HRESULT CCamera::InitGameObject_Clone(void* _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	// ī�޶��� �ʱ� ������ �����ϱ� ���� ����ü
	m_tCameraDesc = *reinterpret_cast<CAMERA_DESC*>(_pArgument);
	
	// Transform ������Ʈ�� �̵��ӵ�, ȸ���ӵ� �����ϱ� ���� ����ü ����
	CTransform::TRANSFORM_DESC tTransformDesc = m_tCameraDesc.tTransformDesc;

	// CTransform ������Ʈ�� Ŭ�� ����(���� �ε����� 0 == STATIC_SCENE���� �־���)
	CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &tTransformDesc);

	// ī�޶� ��ġ �ʱ� ����(CTransform�� ���� ��ȯ ���)�� �����ϱ� ���� ����3 ���� ����
	_vec3 vRight, vUp, vLook, vPosition;

	vPosition = m_tCameraDesc.vEye;				//vEye�� ī�޶��� ��ġ
	vLook = m_tCameraDesc.vAt - vPosition;		//ī�޶� ���� �� - ī�޶��� ��ġ = ī�޶� ���� ����
	
	// ������� Y��� vLook�� �����Ͽ� �׿� ������ ����, vRight�� ����
	D3DXVec3Cross(&vRight, &m_tCameraDesc.vAxisY, &vLook);
	// vLook�� vRight�� �����Ͽ� vUp�� ����
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	// vPosition�� ������ ������ �� ���͸� ����ȭ ��
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);
	/* ī�޶�� Scale�� ��� ����ȭ �س������� ����ϱ� ���� */
	
	//CTransform ������Ʈ�� �ִ� ������Ŀ� �� ���͵� ����
	m_pTransformCom->SetState(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->SetState(CTransform::STATE_UP, vUp);
	m_pTransformCom->SetState(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPosition);
	
	// �� ��ȯ ��� ����(ī�޶��� ���� ��ȯ ����� ����ķ� ��ȯ�� ��)
	m_tViewMatrix = m_pTransformCom->GetWorldMatrixInverse();
	// ���� ��� ����(CAMERA_DESC�� ������)
	::D3DXMatrixPerspectiveFovLH(&m_tProjectionMatrix, m_tCameraDesc.fFovY, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
	
	return NOERROR;
}

// Update
HRESULT CCamera::UpdateGameObject(_double _dDeltaTime){
	// ���������ο� �� ��ȯ ��İ� ���� ��� ����
	m_pPipeline->SetTransform(D3DTS_VIEW, m_tViewMatrix = m_pTransformCom->GetWorldMatrixInverse());
	m_pPipeline->SetTransform(D3DTS_PROJECTION, m_tProjectionMatrix);
	m_pPipeline->SetCameraWorldMatrix(m_pTransformCom->GetWorldMatrix());

	return NOERROR;
}

// LateUpdate
HRESULT CCamera::LateUpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

// Render
HRESULT CCamera::RenderGameObject(){
	return NOERROR;
}

// LateInitialize
HRESULT CCamera::LateInitGameObject(){
	return NOERROR;
}



// Free
void CCamera::Free(){
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pPipeline);

	CGameObject::Free();
}
