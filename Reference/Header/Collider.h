#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "Component.h"

BEGIN(Engine)

class CShader;
class CTransform;
class CPipeline;
class CCollisionManager;
class CGameObject;

class ENGINE_DLL CCollider final :
	public CComponent{
public:
	// 충돌 타입 열거체
	enum COLL_TYPE{
		COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE
	};
	// 콜라이더 세부설정 구조체, 구충돌 콜라이더의 경우 x만 읽는다.
	typedef struct tagColliderDesc{
		CGameObject* pGameObj;
		_vec3 vScale;
		_vec3 vLocalPosition;
		_matrix* pParentWorldMatrix;
	}COLLIDER_DESC;
	// OBB를 체크하기 한 정보를 담기 위한 구조체
	typedef struct tagOBBDesc{
		_vec3 vProjAxis[3];
		_vec3 vAlignAxis[3];
		_vec3 vCenterPos;
	}OBB_DESC;
private:
	// 생성자
	explicit CCollider(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CCollider(const CCollider& _Collider);
	// 소멸자
	virtual ~CCollider() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(COLL_TYPE _eType);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;

public:

public:
	// 충돌 매니저에 콜라이더 등록
	HRESULT SettingToCollisionManager();
	// 콜라이더의 업데이트
	HRESULT UpdateCollider();
	// 콜라이더의 렌더
	HRESULT RenderCollider();
	// 충돌 매니저에서 콜라이더 삭제
	HRESULT DeleteToCollisionManager();
	// 충돌 체크
	_bool IsCollision();

	// Get
public:
	_uint GetColliderType(){
		return m_eType;
	}
	_bool IsEnable(){
		return m_bIsEnable;
	}
	LPD3DXMESH GetMesh(){
		return m_pMesh;
	}
	_vec3 GetPosition(){
		return _vec3(m_tWorldMatrix.m[3]);
	}
	CGameObject* GetCollisionObject(){
		return m_pCollisionObject;
	}
	const _matrix* GetWorldMatrixPointer() const{
		return &m_tWorldMatrix;
	}

private:
	// 콜라이더의 셰이더
	CShader* m_pColliderShader = nullptr;
	// 변환 행렬들을 얻어오기 위한 파이프라인 변수
	CPipeline* m_pPipeline = nullptr;
	// 충돌 타입
	COLL_TYPE m_eType;
	// 충돌 컴포넌트의 세부설정 값
	COLLIDER_DESC m_tColliderDesc;
	// 콜라이더 메쉬
	LPD3DXMESH m_pMesh = nullptr;
	// 콜라이더 메쉬의 어드전시
	LPD3DXBUFFER m_pAdjacency = nullptr;

	// 콜라이더의 월드행렬
	_matrix m_tWorldMatrix;

	// AABB의 충돌을 검사하는 변수
	_vec3 m_vMin_AABB;
	_vec3 m_vMax_AABB;

	// OBB를 검사하기위한 구조체(구충돌은 필요 없으니 AABB와 OBB일때만 동적 할당)
	OBB_DESC* m_pOBB_Desc = nullptr;

	// 충돌 확인 변수
	_bool m_bIsCollision = false;
	// 충돌된 객체
	CGameObject* m_pCollisionObject = nullptr;
	// 충돌 검사 여부
	_bool m_bIsEnable = false;

public:
	static CCollider* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, COLL_TYPE _eType);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();

	friend CCollisionManager;
};

END

#endif // !__COLLIDER_H__