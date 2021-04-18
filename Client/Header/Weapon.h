#ifndef __WEAPON_H__
#define	__WEAPON_H__

#include "Component.h"
#include "Management.h"

#include "..\Header\Player.h"
#include "..\Header\NonPlayer.h"


BEGIN(Engine)
class CShader;
class CTransform;
class CDynamicMesh;
class CStaticMesh;
class CPicker;
class CTimer;
class CEffect;
END

BEGIN(Client)

class CWeapon abstract : public CComponent{
public:
	// 무기종류를 구분하는 열거체
	enum WEAPON_TYPE{
		AR, SMG, LMG, SR, SG, PS, GL
	};
	enum GRENADE_ANI_SET{
		GRENADE_THROW					   = 0,
		GRENADE_THROWING_IDLE,			// = 1
		GRENADE_PULLPIN					// = 2
	};

protected:
	explicit CWeapon(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CWeapon(const CWeapon& _Weapon);
	virtual ~CWeapon() = default;

public:
	void SetThirdPerson(_bool _bIsThirdPerson){
		m_bIsThirdPerson = _bIsThirdPerson;
	}
	void SetPlayerFlag(_byte* _pPlayerFlag){
		m_pPlayerFlag = _pPlayerFlag;
	}
	void SetPlayerPicker(CPicker* _pPicker){
		m_pPlayerPicker = _pPicker;
	}
	void SetLateInit(_bool _bIsLateInit){
		m_bIsLateInit = _bIsLateInit;
	}
	void AnimationReset(){
		m_pFirstPersonModelCom->SetUpAnimationSet(0);
		m_pFirstPersonModelCom->ResetTime();
	}

public:
	virtual _int GetCurrentMagazine() const{
		return m_iCurrentMagazine;
	}
	virtual _int GetCurrentAmmo() const{
		return m_iCurrentAmmo;
	}

protected:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

public:
	// LateInitialize
	virtual HRESULT LateInitWeapon() PURE;
	// Update
	virtual HRESULT UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime);
	// Render
	virtual HRESULT RenderWeapon();

public:
	WEAPON_TYPE GetWeaponType(){
		return m_eWeaponType;
	}
	CDynamicMesh* GetThirdPersonModelWeapon(){
		return m_pThirdPersonModelCom;
	}
	const _tchar* GetName(){
		return m_szWaeponName;
	}

protected:
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;

	// 플레이어의 Picker 컴포넌트
	const CPicker* m_pPlayerPicker = nullptr;

	// 1인칭으로 보여줄 메쉬
	CDynamicMesh* m_pFirstPersonModelCom = nullptr;
	// 무기 종류
	WEAPON_TYPE m_eWeaponType;
	// 무기 이름
	_tchar* m_szWaeponName = nullptr;
	// 데미지
	_float m_fDamage = 0.f;
	// 한 탄창당 최대 탄약
	_int m_iMaxOneMagazine = 0;
	// 현재 탄창에 남은 탄약
	_int m_iCurrentMagazine = 0;
	// 최대로 가질수 있는 탄약 수
	_int m_iMaxAmmo = 0;
	// 현재 남아있는 탄약 수
	_int m_iCurrentAmmo = 0;
	// 발사 속도
	_float m_fFiringRate = 0.f;
	// 발사속도 체크를 위한 타이머
	CTimer* m_pFiringTimer = nullptr;
	
	// 발사 시간 누적을 위한 변수
	_double m_dAccFiringTime = 0.0;

	// 애니메이션 시간 누적을 위한 변수
	_double m_dAnimationTime = 0.0;

	// 1인칭 근접공격시 보여주는 칼
	CStaticMesh* m_pFirstPersonKnifeCom = nullptr;
	// 칼을 쥐고있는 손의 뼈의 변환행렬
	_matrix* m_pKnifeFrameUpdateMatrix = nullptr;
	// 현재 상태가 칼이 보이는 상태인지 확인하는 변수
	_bool m_bIsMeleeAttack = false;

	// 1인칭 야간투시경착용시 보이는 야간투시경 메쉬
	CStaticMesh* m_pFirstPersonNVGCom = nullptr;
	// 야간투시경의 뼈 변환 행렬
	_matrix* m_pNVGFrameUpdateMatrix = nullptr;
	// 현재 상태가 야간투시경이 보이는 상태인지 확인
	_bool m_bIsNVG = false;

	// 수류탄을 던지는 애니메이션이 들어있는 다이나믹 메쉬
	CDynamicMesh* m_pVeiwmodelGrenade = nullptr;
	// 수류탄을 던지는 상태인지 확인하는 변수
	_bool m_bIsGrenade = false;

	// 3인칭으로 보여줄 메쉬
	CDynamicMesh* m_pThirdPersonModelCom = nullptr;
	// 현재 보여줘야 하는 상태가 3인칭인지 확인하는 변수
	_bool m_bIsThirdPerson = false;

	_byte* m_pPlayerFlag = nullptr;

	_bool m_bIsLateInit = false;

	CEffect* m_pMuzzleEffect;
	_bool m_bIsFire = false;
	_matrix* m_pMuzzleFlashMatrix = nullptr;
	CLight* m_pMuzzleLight = nullptr;

public:
	virtual HRESULT ResetAnimationSound() = 0;

protected:
	HRESULT Fire(const POINT& pt, CPicker* _pPicker);
	HRESULT Reload();
	_bool Grenade();
	HRESULT UpdateGrenadeState(_double _dDeltaTime);

	//HRESULT KnifeUpdate();

	virtual HRESULT WeaponControl(CPicker* _pPicker) PURE;
	virtual HRESULT UpdateWeaponState(_double _dDeltaTime) PURE;

private:
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	_uint m_uiShaderPass = 0;

private:
	HRESULT FirstPersonModelRender();
	HRESULT FirstPersonKnifeRender();
	//HRESULT FirstPersonNVGRender();
	
private:
	// 셰이더에 세팅
	HRESULT SettingShader();
	HRESULT SettingKnifeShader();
	//HRESULT SettingNVGShader();

public:
	virtual void Free();
};

END

#endif // !__WEAPON_H__