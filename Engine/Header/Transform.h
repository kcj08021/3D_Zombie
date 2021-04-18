#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)
class CNavigation;

class ENGINE_DLL CTransform final: 
	public CComponent{
public:
	// 월드 변환 행렬의 상태 열거체
	enum STATE_TYPE{ STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION };
	// 복제시 객체 세부설정 구조체(pArgument로 받아올 구조체)
	typedef struct tagTransformDesc{
		_matrix*	pMatrixWorldSetting;
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;
private:
	// 생성자
	explicit CTransform(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CTransform(const CTransform& _Transform);
	// 소멸자
	virtual ~CTransform() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

	// Get
public:
	// 월드 변환 행렬의 상태를 얻어옴
	_vec3 GetState(STATE_TYPE _eState) const{
		return _vec3(m_matWorld.m[_eState]);
	}
	// 월드 변환 행렬을 얻어옴
	_matrix GetWorldMatrix() const{
		return m_matWorld;
	}
	_matrix* GetWorldMatrixPointer(){
		return &m_matWorld;
	}
	// 월드 변환 행렬의 역행렬을 얻어옴
	_matrix GetWorldMatrixInverse() const;

	// Set
public:
	// 월드 변환 행렬에 상태를 적용
	void SetState(STATE_TYPE _eState, const _vec3& vStateData){
		::memcpy(m_matWorld.m[_eState], &vStateData, sizeof(_vec3));
	}
	// 월드변환 행렬 교체
	void SetWorldMatrix(const _matrix& _matWorld){
		m_matWorld = _matWorld;
	}
	// 세부설정 적용
	void SetTransformDesc(const TRANSFORM_DESC& _tTransformDesc){
		m_TransformDecs = _tTransformDesc;
	}

public:
	// 특정 방향으로 이동
	HRESULT Move(_double _dDeltaTime, _vec3& _vDir, CNavigation* _pNavigation = nullptr);
	// 앞으로 이동
	HRESULT MoveStraight(_double _dDeltaTime);
	// 뒤로 이동
	HRESULT MoveBackward(_double _dDeltaTime);
	// 좌로 이동
	HRESULT MoveLeft(_double _dDeltaTime);
	// 우로 이동
	HRESULT MoveRight(_double _dDeltaTime);
	// Right 축을 기준으로 회전
	HRESULT RotationRightAxis(_double _dDeltaTime);
	// 특정 축을 기준으로 회전
	HRESULT RotationAxis(const _vec3* _pAxis, _double _dDeltaTime);

private:
	// 월드 변환 행렬
	_matrix m_matWorld;
	// 복제시 객체 세부설정 구조체(복제시 초기화 해야함)
	TRANSFORM_DESC m_TransformDecs;

	_float fRotateRightAxisAcc = 0.f;

public:
	// 원본 생성 함수(static)
	static CTransform* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};
END

#endif // !__TRANSFORM_H__


