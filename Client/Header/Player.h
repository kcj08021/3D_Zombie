#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "..\Header\Client_Headers.h"
#include "Camera.h"

BEGIN(Engine)
class CStaticMesh;
END

BEGIN(Client)

class CTerrain;
class CWeapon;


class CPlayer final 
	: public CCamera{
private:
	enum WEAPON_SELECT{
		PRIMARY_WEAPON, SECONDARY_WEAPON, GRENADE_LAUNCHER1, GRENADE_LAUNCHER2, WEAPON_END
	};
public:
	// 플레이어의 상태 플래그 열거체
	enum PLAYER_FALG{
		FLAG_SPRINT					= 0x01,
		FLAG_CROUCH					= 0x02,
		FLAG_PRONE					= 0x04,
		FLAG_NVG					= 0x08,
		FLAG_JUMP_START				= 0x10,
		FLAG_JUMP_END				= 0X20,
		FLAG_WEAPON_CHANGE			= 0X40,
		FLAG_ADS					= 0x80
	};
public:
	typedef struct tagPlayerDesc{
		CCamera::CAMERA_DESC tCameraDesc;
	}PLAYER_DESC;

private:
	// 생성자
	explicit CPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CPlayer(const CPlayer& _Player);
	// 소멸자
	virtual ~CPlayer() = default;

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
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

private:
	// 플레이어 상태 플래그
	_byte m_bytePlayerFlag = 0x00;
	// 플레이어 체력
	_float m_fPlayerHP = c_fPlayerFullHP;
	// 회복 시간 누적
	_double m_dPlayerHealingTimeAcc = 0.0;

	// 무기 오브젝트 배열(2개 소지가능)
	CWeapon* m_pWeapon[WEAPON_END];
	// 현재 선택된 무기의 인덱스
	WEAPON_SELECT m_eWeaponType;

	// 근접공격 콜라이더 컴포넌트
	CCollider* m_pMeleepColliderCom = nullptr;
	// 피격 콜라이더 컴포넌트
	CCollider* m_pHitColliderCom = nullptr;

	// 무기 오브젝트 콜라이더 컴포넌트
	CCollider* m_pWeaponObjectColliderCom = nullptr;
	// 무기 오브젝트 콜라이더를 위한 월드 행렬
	_matrix m_tWeaponObjectColliderMatrix;

	// 네비게이션 컴포넌트
	CNavigation* m_pNavigationCom = nullptr;
	// 렌더러 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 픽커 컴포넌트
	CPicker* m_pPickerCom = nullptr;

	// 중력가속도 누적값
	_float m_fGravityAcc = 0.f;

	// 이전 프레임에서 이동한 거리와 방향
	_vec3 m_vPrePos;

	// 점프 누적 시간
	_double m_dJumpTime = 0.f;

	// 걷기 누적 시간
	_double m_dWalkTime = 0.f;
	// 이동이 성공했는지 확인
	_bool m_bIsWalk;

	/* 플레이어 피격시 이용되는 변수들 */
	// 피격시 흔들리는 카메라의 행렬
	_matrix m_matShakeCamera;
	// 카메라 흔들리는 정도를 보관하는 변수
	_float m_fPower;
	// 카메라 흔들림이 시작됬는지 체크
	_bool m_bIsCameraShaking = false;
	// 카메라 흔들림이 끝나고 원상복귀 중인지 체크
	_bool m_bIsCameraShakingEnd = false;
	// 카메라 흔들림 시간 누적 변수
	_double m_dCameraShakingTimeAcc = 0.0;

	// 플레이어 죽음 체크
	_bool m_bIsDead = false;

private:
	// 시작시 설정되는 무기를 세팅
	HRESULT WeaponSetting();
	// 플레이어 조작
	void PlayerControl(_double _dDeltaTime);
	// 점프
	void Jump(_double _dDeltaTime);
	// 플레이어에 대한 자유낙하 계산
	void LandingGround(_double _dDeltaTime);
	// 카메라 흔들기
	void HitCamera(_double _dDeltaTime, _float _fPower);
	// 플레이어의 죽음 설정
	void SetDead();


public:
	// 원본 생성 함수(static)
	static CPlayer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};

END
#endif // !__PLAYER_H__
