#include "stdafx.h"
#include "..\Header\ToolCamera.h"

USING(Tool)

// 생성자
CToolCamera::CToolCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CCamera(_pGraphicDevice){
}

// 복사 생성자
CToolCamera::CToolCamera(const CToolCamera & _ToolCamera)
	: CCamera(_ToolCamera){
}

// Initialize with Prototype
HRESULT CToolCamera::InitGameObject_Proto(){
	return CCamera::InitGameObject_Proto();
}

// Initialize with Clone
HRESULT CToolCamera::InitGameObject_Clone(void * _pArgument){
	return CCamera::InitGameObject_Clone(_pArgument);
}

// LateInitialize
HRESULT CToolCamera::LateInitGameObject(){
	return CCamera::LateInitGameObject();
}

// Update
HRESULT CToolCamera::UpdateGameObject(_double _dDeltaTime){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 키 입력
	if(pManagement->IsKeyPressing(DIK_W)){
		m_pTransformCom->MoveStraight(_dDeltaTime);
	}
	if(pManagement->IsKeyPressing(DIK_S)){
		m_pTransformCom->MoveBackward(_dDeltaTime);
	}
	if(pManagement->IsKeyPressing(DIK_A)){
		m_pTransformCom->MoveLeft(_dDeltaTime);
	}
	if(pManagement->IsKeyPressing(DIK_D)){
		m_pTransformCom->MoveRight(_dDeltaTime);
	}

	if(pManagement->IsKeyPressing(DIK_Q)){
		_vec3 vLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
		m_pTransformCom->RotationAxis(&vLook, _dDeltaTime);
	}

	if(pManagement->IsKeyPressing(DIK_E)){
		_vec3 vLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);
		m_pTransformCom->RotationAxis(&vLook, -_dDeltaTime);
	}

	// 마우스 움직임
	_long lMouseMove = 0;
	if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_MBUTTON)){
		if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_X)){
			_vec3 vWorldUp(0.f, 1.f, 0.f);
			m_pTransformCom->RotationAxis(&vWorldUp, _dDeltaTime * lMouseMove * 0.1);
		}

		if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_Y)){
			_vec3 vRight = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
			m_pTransformCom->RotationAxis(&vRight, _dDeltaTime * lMouseMove * 0.1);
		}
	}

	SafeRelease(pManagement);

	return CCamera::UpdateGameObject(_dDeltaTime);

}

// LateUpdate
HRESULT CToolCamera::LateUpdateGameObject(_double _dDeltaTime){
	return CCamera::LateUpdateGameObject(_dDeltaTime);
}

// Render
HRESULT CToolCamera::RenderGameObject(){
	return CCamera::RenderGameObject();
}

HRESULT CToolCamera::SetFocusPos(const _vec3 & _vPos){
	_vec3 vRight, vUp, vLook, vPosition;

	vPosition = _vPos + _vec3(0.f, 10.f, -10.f);
	vLook = _vPos - vPosition;		//카메라가 보는 점 - 카메라의 위치 = 카메라가 보는 각도

	// 월드상의 Y축과 vLook을 외적하여 그에 직교한 벡터, vRight를 구함
	D3DXVec3Cross(&vRight, &_vec3(0.f,0.1f,0.f), &vLook);
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

	return NOERROR;
}

// 원본 생성 함수(static)
CToolCamera * CToolCamera::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CToolCamera* pInstance = new CToolCamera(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CToolCamera Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject * CToolCamera::CreateClone(void * _pArgument){
	CToolCamera* pInstance = new CToolCamera(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CToolCamera Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CToolCamera::Free(){
	CCamera::Free();
}