// Client 헤더
#include "stdafx.h"
#include "..\Header\FreeCamera.h"

// Reference 헤더
#include "Management.h"

USING(Client)

// 생성자
CFreeCamera::CFreeCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CCamera(_pGraphicDevice){
}

// 복사 생성자
CFreeCamera::CFreeCamera(const CFreeCamera & _FreeCamera)
	: CCamera(_FreeCamera){
}

// Initialize with Prototype
HRESULT CFreeCamera::InitGameObject_Proto(){
	return NOERROR;
}

// Initialize with Clone
HRESULT CFreeCamera::InitGameObject_Clone(void* _pArgument){
	return CCamera::InitGameObject_Clone(_pArgument);
}

// LateInitialize
HRESULT CFreeCamera::LateInitGameObject(){
	return NOERROR;
}

// Update
HRESULT CFreeCamera::UpdateGameObject(_double _dDeltaTime){
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

	if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_X)){
		_vec3 vWorldUp(0.f, 1.f, 0.f);
		m_pTransformCom->RotationAxis(&vWorldUp, _dDeltaTime * lMouseMove * 0.1);
	}
	
	if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_Y)){
		m_pTransformCom->RotationRightAxis(_dDeltaTime * lMouseMove * 0.1);
	}

	SafeRelease(pManagement);

	return CCamera::UpdateGameObject(_dDeltaTime);
}

// LateUpdate
HRESULT CFreeCamera::LateUpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

// Render
HRESULT CFreeCamera::RenderGameObject(_double _dDeltaTime){
	return NOERROR;
}

// 원본 생성 함수(static)
CFreeCamera * CFreeCamera::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CFreeCamera* pInstance = new CFreeCamera(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CFreeCamera Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject * CFreeCamera::CreateClone(void* _pArgument){
	CFreeCamera* pInstance = new CFreeCamera(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CFreeCamera Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// Free
void CFreeCamera::Free(){
	CCamera::Free();
}
