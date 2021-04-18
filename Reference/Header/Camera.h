#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GameObject.h"
#include "..\Header\Transform.h"
#include "..\Header\Pipeline.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CCamera abstract:
	public CGameObject{
public:
	typedef struct tagCameraDesc{
		// 카메라 관련 관련 정보
		_vec3 vEye;		// 카메라 위치
		_vec3 vAt;		// 카메라가 보는 점
		_vec3 vAxisY;	// 월드 상의 Y축 벡터
		// 투영행렬 관련 정보
		_float fFovY;	// 시야각
		_float fAspect;	// 화면비
		_float fNear;	// 투영시 최소 거리
		_float fFar;	// 투영시 최대 거리
		// TRANSFORM_DESC
		CTransform::TRANSFORM_DESC tTransformDesc;
	}CAMERA_DESC;
protected:
	// 생성자
	explicit CCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CCamera(const CCamera& _Camera);
	// 소멸자
	virtual ~CCamera() = default;

public:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject();

protected:
	// Transform 컴포넌트(월드 변환 행렬)
	CTransform* m_pTransformCom = nullptr;
	// CPipeline 변수
	CPipeline* m_pPipeline = nullptr;

protected:
	CAMERA_DESC m_tCameraDesc;
	_matrix m_tViewMatrix;
	_matrix m_tProjectionMatrix;



public:
	// 복제 생성 함수
	virtual CGameObject * CreateClone(void* _pArgument) override PURE;
public:
	// Free
	virtual void Free();
};
END

#endif // !__CAMERA_H__
