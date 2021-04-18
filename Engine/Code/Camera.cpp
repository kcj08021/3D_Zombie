#include "..\Header\Camera.h"

// 생성자
CCamera::CCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pTransformCom(nullptr)
	, m_pPipeline(CPipeline::GetInstance()){
	SafeAddRef(m_pPipeline);
	::D3DXMatrixIdentity(&m_tViewMatrix);
	::D3DXMatrixIdentity(&m_tProjectionMatrix);
}

// 복사 생성자
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

	// 카메라의 초기 설정을 설정하기 위한 구조체
	m_tCameraDesc = *reinterpret_cast<CAMERA_DESC*>(_pArgument);
	
	// Transform 컴포넌트에 이동속도, 회전속도 전달하기 위한 구조체 생성
	CTransform::TRANSFORM_DESC tTransformDesc = m_tCameraDesc.tTransformDesc;

	// CTransform 컴포넌트의 클론 생성(씬의 인덱스는 0 == STATIC_SCENE으로 넣어줌)
	CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &tTransformDesc);

	// 카메라 위치 초기 설정(CTransform의 월드 변환 행렬)을 설정하기 위한 벡터3 변수 선언
	_vec3 vRight, vUp, vLook, vPosition;

	vPosition = m_tCameraDesc.vEye;				//vEye는 카메라의 위치
	vLook = m_tCameraDesc.vAt - vPosition;		//카메라가 보는 점 - 카메라의 위치 = 카메라가 보는 각도
	
	// 월드상의 Y축과 vLook을 외적하여 그에 직교한 벡터, vRight를 구함
	D3DXVec3Cross(&vRight, &m_tCameraDesc.vAxisY, &vLook);
	// vLook과 vRight를 외적하여 vUp을 구함
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	// vPosition을 제외한 나머지 세 벡터를 정규화 함
	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);
	/* 카메라는 Scale이 없어서 정규화 해놓는편이 사용하기 편함 */
	
	//CTransform 컴포넌트에 있는 월드행렬에 각 벡터들 설정
	m_pTransformCom->SetState(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->SetState(CTransform::STATE_UP, vUp);
	m_pTransformCom->SetState(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPosition);
	
	// 뷰 변환 행렬 생성(카메라의 월드 변환 행렬을 역행렬로 변환한 값)
	m_tViewMatrix = m_pTransformCom->GetWorldMatrixInverse();
	// 투영 행렬 생성(CAMERA_DESC의 값으로)
	::D3DXMatrixPerspectiveFovLH(&m_tProjectionMatrix, m_tCameraDesc.fFovY, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
	
	return NOERROR;
}

// Update
HRESULT CCamera::UpdateGameObject(_double _dDeltaTime){
	// 파이프라인에 뷰 변환 행렬과 투영 행렬 전달
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
