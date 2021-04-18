#include "..\Header\Transform.h"
#include "..\Header\Navigation.h"

// 생성자
CTransform::CTransform(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice){
	::D3DXMatrixIdentity(&m_matWorld);
	::ZeroMemory(&m_TransformDecs, sizeof(m_TransformDecs));
}

// 복사 생성자
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
		// 받아온 pArgument로 객체 세부설정 구조체 초기화
		::memcpy(&m_TransformDecs, _pArgument, sizeof(TRANSFORM_DESC));
		if(nullptr != m_TransformDecs.pMatrixWorldSetting){
			m_matWorld = *m_TransformDecs.pMatrixWorldSetting;
		}
	}

	return NOERROR;
}

// 월드 변환 행렬의 역행렬을 얻어옴
_matrix CTransform::GetWorldMatrixInverse() const{
	_matrix matWorldInv = {};

	// 역행렬 계산
	::D3DXMatrixInverse(&matWorldInv, nullptr, &m_matWorld);
	
	return matWorldInv;
}
// 특정 방향으로 이동
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

// 앞으로 이동
HRESULT CTransform::MoveStraight(_double _dDeltaTime){
	// 월드 변환 행렬에서 현재 위치(4행)과 Look 축 벡터(3행) 가져옴
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// Look축 벡터를 정규화 한 뒤, 이동 속도와 프레임 당 시간을 곱한뒤 위치 벡터에 더해줌
	vPosition += *::D3DXVec3Normalize(&vLook, &vLook)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// 월드 변환 행렬에 위치(4행)에 계산한 벡터 설정
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// 뒤로 이동
HRESULT CTransform::MoveBackward(_double _dDeltaTime){
	// 월드 변환 행렬에서 현재 위치(4행)과 Look 축 벡터(3행) 가져옴
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// Look축 벡터를 정규화 한 뒤, 이동 속도와 프레임 당 시간을 곱한뒤 위치 벡터에 빼줌(뒤로 이동)
	vPosition -= *::D3DXVec3Normalize(&vLook, &vLook)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// 월드 변환 행렬에 위치(4행)에 계산한 벡터 설정
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// 좌로 이동
HRESULT CTransform::MoveLeft(_double _dDeltaTime){
	// 월드 변환 행렬에서 현재 위치(4행)과 Right 축 벡터(1행) 가져옴
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);

	// Right축 벡터를 정규화 한 뒤, 이동 속도와 프레임 당 시간을 곱한뒤 위치 벡터에 빼줌(좌측으로 이동)
	vPosition -= *::D3DXVec3Normalize(&vRight, &vRight)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// 월드 변환 행렬에 위치(4행)에 계산한 벡터 설정
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

// 우로 이동
HRESULT CTransform::MoveRight(_double _dDeltaTime){
	// 월드 변환 행렬에서 현재 위치(4행)과 Right 축 벡터(1행) 가져옴
	_vec3 vPosition(m_matWorld.m[STATE_POSITION]);
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);

	// Right축 벡터를 정규화 한 뒤, 이동 속도와 프레임 당 시간을 곱한뒤 위치 벡터에 더해줌
	vPosition += *::D3DXVec3Normalize(&vRight, &vRight)* m_TransformDecs.fSpeedPerSec * static_cast<_float>(_dDeltaTime);

	// 월드 변환 행렬에 위치(4행)에 계산한 벡터 설정
	SetState(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

HRESULT CTransform::RotationRightAxis(_double _dDeltaTime){
	_matrix matRotation = {};

	// 현재 월드 변환 행렬의 각 축 벡터 가져옴
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);
	_vec3 vUp(m_matWorld.m[STATE_UP]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	_float RatoteValue = m_TransformDecs.fRotationPerSec * static_cast<float>(_dDeltaTime);
	if((D3DX_PI * 0.5f) < fabs(fRotateRightAxisAcc + RatoteValue))
		RatoteValue = RatoteValue < 0.f ? D3DX_PI * -0.5f - fRotateRightAxisAcc : D3DX_PI * 0.5f - fRotateRightAxisAcc;
	
	// 회전 행렬 계산
	::D3DXMatrixRotationAxis(&matRotation, &vRight, RatoteValue);
	fRotateRightAxisAcc += RatoteValue;

	// 월드 변환 행렬의 각 축(벡터)에 회전 행렬 곱함
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_RIGHT]), &vRight, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_UP]), &vUp, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_LOOK]), &vLook, &matRotation);

	return NOERROR;
}

// 특정 축을 기준으로 회전
HRESULT CTransform::RotationAxis(const _vec3* _pAxis, _double _dDeltaTime){
	_matrix matRotation = {};

	// 회전 행렬 계산
	::D3DXMatrixRotationAxis(&matRotation, _pAxis, m_TransformDecs.fRotationPerSec * static_cast<float>(_dDeltaTime));

	// 현재 월드 변환 행렬의 각 축 벡터 가져옴
	_vec3 vRight(m_matWorld.m[STATE_RIGHT]);
	_vec3 vUp(m_matWorld.m[STATE_UP]);
	_vec3 vLook(m_matWorld.m[STATE_LOOK]);

	// 월드 변환 행렬의 각 축(벡터)에 회전 행렬 곱함
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_RIGHT]), &vRight, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_UP]), &vUp, &matRotation);
	::D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(m_matWorld.m[STATE_LOOK]), &vLook, &matRotation);

	return NOERROR;
}

// 원본 생성 함수(static)
CTransform * CTransform::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CTransform* pInstance = new CTransform(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		MSG_BOX(L"CTransform Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// 복제 생성 함수
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
