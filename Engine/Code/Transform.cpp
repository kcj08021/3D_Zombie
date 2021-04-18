#include "..\Header\Transform.h"
#include "..\Header\Navigation.h"

// ������
CTransform::CTransform(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice){
	::D3DXMatrixIdentity(&m_matWorld);
	::ZeroMemory(&m_TransformDecs, sizeof(m_TransformDecs));
}

// ���� ������
CTransform::CTransform(const CTransform & _Transform)
	: CComponent(_Transform)
	, m_matWorld(_Transform.m_matWorld)
	, m_TransformDecs(_Transform.m_TransformDecs){
}

// Initialize with Prototype
HRESULT CTransform::InitComponent_Proto(){

	return NOERROR;
}

// Initialize with Clone
HRESULT CTransform::InitComponent_Clone(void* _pArgument){
	if(nullptr != _pArgument){
		// �޾ƿ� pArgument�� ��ü ���μ��� ����ü �ʱ�ȭ
		::memcpy(&m_TransformDecs, _pArgument, sizeof(TRANSFORM_DESC));
		if(nullptr != m_TransformDecs.pMatrixWorldSetting){
			m_matWorld = *m_TransformDecs.pMatrixWorldSetting;
		}
	}

	return NOERROR;
}

// ���� ��ȯ ����� ������� ����
_matrix CTransform::GetWorldMatrixInverse() const{
	_matrix matWorldInv = {};

	// ����� ���
	::D3DXMatrixInverse(&matWorldInv, nullptr, &m_matWorld);
	
	return matWorldInv;
}
// Ư�� �������� �̵�
HRESULT CTransform::Move(_double _dDeltaTime, _vec3& _vDir, CNavigation* _pNavigation/* = nullptr*/){
	_vec3 vPosition(m_matWorld.m[3]);

	vPosition += _vDir*m_TransformDecs.fSpeedPerSec* static_cast<_float>(_dDeltaTime);

	if(nullptr == _pNavigation){
		memcpy(m_matWorld.m[3], vPosition, sizeof(_vec3));
	} else{
		if(_pNavigation->MovingCheck(vPosition))
			memcpy(m_matWorld.m[3], vPosition, sizeof(_vec3));
	}
	
	return NOERROR;
}

// ������ �̵�
HRESULT CTransform::MoveStraight(_double _dDeltaTime){
	// ���� ��ȯ ��Ŀ��� ���� ��ġ(4��)�� Look �� ����(3��) ������
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// Look�� ���͸� ����ȭ �� ��, �̵� �ӵ��� ������ �� �ð��� ���ѵ� ��ġ ���Ϳ� ������
	vPosition += *::D3DXVec3Normalize(&vLook, &vLook)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// ���� ��ȯ ��Ŀ� ��ġ(4��)�� ����� ���� ����
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// �ڷ� �̵�
HRESULT CTransform::MoveBackward(_double _dDeltaTime){
	// ���� ��ȯ ��Ŀ��� ���� ��ġ(4��)�� Look �� ����(3��) ������
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// Look�� ���͸� ����ȭ �� ��, �̵� �ӵ��� ������ �� �ð��� ���ѵ� ��ġ ���Ϳ� ����(�ڷ� �̵�)
	vPosition -= *::D3DXVec3Normalize(&vLook, &vLook)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// ���� ��ȯ ��Ŀ� ��ġ(4��)�� ����� ���� ����
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// �·� �̵�
HRESULT CTransform::MoveLeft(_double _dDeltaTime){
	// ���� ��ȯ ��Ŀ��� ���� ��ġ(4��)�� Right �� ����(1��) ������
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);

	// Right�� ���͸� ����ȭ �� ��, �̵� �ӵ��� ������ �� �ð��� ���ѵ� ��ġ ���Ϳ� ����(�������� �̵�)
	vPosition -= *::D3DXVec3Normalize(&vRight, &vRight)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// ���� ��ȯ ��Ŀ� ��ġ(4��)�� ����� ���� ����
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// ��� �̵�
HRESULT CTransform::MoveRight(_double _dDeltaTime){
	// ���� ��ȯ ��Ŀ��� ���� ��ġ(4��)�� Right �� ����(1��) ������
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);

	// Right�� ���͸� ����ȭ �� ��, �̵� �ӵ��� ������ �� �ð��� ���ѵ� ��ġ ���Ϳ� ������
	vPosition += *::D3DXVec3Normalize(&vRight, &vRight)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// ���� ��ȯ ��Ŀ� ��ġ(4��)�� ����� ���� ����
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

HRESULT CTransform::RotationRightAxis(_double _dDeltaTime){
	_matrix matRotation = {};

	// ���� ���� ��ȯ ����� �� �� ���� ������
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);
	_vec3 vUp(m_matWorld.m[STATE_UP]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	_float RatoteValue = m_TransformDecs.fRotationPerSec * static_cast<float>(_dDeltaTime);
	if((D3DX_PI * 0.5f) < fabs(fRotateRightAxisAcc + RatoteValue))
		RatoteValue = RatoteValue < 0.f ? D3DX_PI * -0.5f - fRotateRightAxisAcc : D3DX_PI * 0.5f - fRotateRightAxisAcc;
	
	// ȸ�� ��� ���
	::D3DXMatrixRotationAxis(&matRotation, &vRight, RatoteValue);
	fRotateRightAxisAcc += RatoteValue;

	// ���� ��ȯ ����� �� ��(����)�� ȸ�� ��� ����
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_RIGHT]), &vRight, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_UP]), &vUp, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_LOOK]), &vLook, &matRotation);

	return NOERROR;
}

// Ư�� ���� �������� ȸ��
HRESULT CTransform::RotationAxis(const _vec3* _pAxis, _double _dDeltaTime){
	_matrix matRotation = {};

	// ȸ�� ��� ���
	::D3DXMatrixRotationAxis(&matRotation, _pAxis, m_TransformDecs.fRotationPerSec * static_cast<float>(_dDeltaTime));

	// ���� ���� ��ȯ ����� �� �� ���� ������
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);
	_vec3 vUp(m_matWorld.m[STATE_UP]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// ���� ��ȯ ����� �� ��(����)�� ȸ�� ��� ����
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_RIGHT]), &vRight, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_UP]), &vUp, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_LOOK]), &vLook, &matRotation);

	return NOERROR;
}

// ���� ���� �Լ�(static)
CTransform * CTransform::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CTransform* pInstance = new CTransform(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		MSG_BOX(L"CTransform Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// ���� ���� �Լ�
CComponent * CTransform::CreateClone(void* _pArgument){
	CComponent* pInstance = new CTransform(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CTransform Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CTransform::Free(){
	CComponent::Free();
}
