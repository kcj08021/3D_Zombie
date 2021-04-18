#include "stdafx.h"
#include "..\Header\ToolCamera.h"

USING(Tool)

// ������
CToolCamera::CToolCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CCamera(_pGraphicDevice){
}

// ���� ������
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

	// Ű �Է�
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

	// ���콺 ������
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
	vLook = _vPos - vPosition;		//ī�޶� ���� �� - ī�޶��� ��ġ = ī�޶� ���� ����

	// ������� Y��� vLook�� �����Ͽ� �׿� ������ ����, vRight�� ����
	D3DXVec3Cross(&vRight, &_vec3(0.f,0.1f,0.f), &vLook);
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

	return NOERROR;
}

// ���� ���� �Լ�(static)
CToolCamera * CToolCamera::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CToolCamera* pInstance = new CToolCamera(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CToolCamera Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// ���� ���� �Լ�
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