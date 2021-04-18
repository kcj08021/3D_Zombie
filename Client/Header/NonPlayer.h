#ifndef __NON_PLAYER_H__
#define __NON_PLAYER_H__

#include "GameObject.h"
#include "..\Header\Client_Headers.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)

class CStaticMeshObject;

class CNonPlayer final :
	public CGameObject{
public:
	typedef struct tagNonPlayerDesc{
		const _tchar* szDynamicMeshProtoName;
		const _tchar* szWeapon;
		CTransform::TRANSFORM_DESC tTransformDesc;
	}NONPLAYER_DESC;

private:
	enum PART_TYPE{
		PARENT, HEAD, BODY_UPPER, BDOY_LOWER, LEFT_SHOULDER, LEFT_ELBOW, RIGHT_SHOULDER, RIGHT_ELBOW, LFET_HIP, LEFT_KNEE, RIGHT_HIP, RIGHT_KNEE, PART_END
	};

	enum ANIMATION_SET{
		ZOMBIE_IDLE					       = 0,
		ZOMBIE_ATTACK,					// = 1
		ZOMBIE_WALK,					// = 2
		ZOMBIE_DEATH				    // = 3			
	};

private:
	// 생성자
	explicit CNonPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CNonPlayer(const CNonPlayer& _NonPlayer);
	// 소멸자
	virtual ~CNonPlayer() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(CStaticMeshObject* _pStaticMesh);
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

public:
	_vec3 GetPosition(){
		return m_PartWorldMatrx[PARENT].m[3];
	}
public:
	void SetStaticMeshMap(CStaticMeshObject* _pStaticMesh){
		m_pStaticMeshMap = _pStaticMesh;
	}
	HRESULT SetExplosionDead(_vec3* vDir);
	_bool CheckHit(const _vec3& _vRayDir, const _vec3& _vRayPos, _float _fDamage);

	/* 원본 생성 멤버 */
private:
	// 렌더러 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 절두체 컴포넌트
	CFrustum* m_pFrustumCom = nullptr;
	// 맵 컴포넌트
	CStaticMeshObject* m_pStaticMeshMap = nullptr;

	/* 복제 생성 컴포넌트 */
private:
	// 다이나믹 메쉬 컴포넌트
	CDynamicMesh* m_pDynamicMeshCom = nullptr;
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;
	// 픽킹 대상 컴포넌트
	CPickee* m_pPickeeCom = nullptr;
	// 네비게이션 컴포넌트
	CNavigation* m_pNavigationCom = nullptr;

private:
	ANIMATION_SET m_eCurrAnimation = ANIMATION_SET::ZOMBIE_IDLE;

	_bool m_bIsDead = false;
	_float m_fHP = c_fZombieFullHP;
	// 죽은뒤 시간이 흐른 정도를 누적
	_float m_fDeadTime = 0.f;

	// 콜라이더가 참조할 각 부위별 월드 매트릭스
	_matrix m_PartWorldMatrx[PART_END];
	// 각 부분의 변환 행렬
	_matrix* m_pPartCombinedMatrix[PART_END];
	// 각 부위별 콜라이더 컴포넌트
	CCollider* m_pPartColliderCom[PART_END];
	// 공격 성공시 중첩 데미지 방지용 변수
	_bool m_bAttackCheck = false;
	// 혈흔 효과를 사용하기 위한 이펙트 포인터
	CEffect* m_pBloodEffect = nullptr;

private:
	// 부위별 행렬과 콜라이더 세팅
	HRESULT SettingPart();
	// 부위별 콜라이더 업데이트
	HRESULT UpdatePart();
	//부위별 콜라이더 렌더
	HRESULT RenderPart();
	// 셰이더의 전역변수 설정
	HRESULT SettingShader();
	// 캐릭터의 현재 상태 설정
	void CheckState();
	// 죽는 상태로 설정
	void SetDeath();
	// 애니메이션 변경을 위한 함수
	void ChangeAnimation(ANIMATION_SET _eAniSet);
	// 이동
	HRESULT CharacterMove(_double _dDeltaTime);
	// 공격
	HRESULT Attack();

public:
	// 원본 생성 함수(static)
	static CNonPlayer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CStaticMeshObject* _pStaticMesh = nullptr);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__NON_PLAYER_H__
