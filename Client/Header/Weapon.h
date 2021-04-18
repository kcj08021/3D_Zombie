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
	// ���������� �����ϴ� ����ü
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
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;

	// �÷��̾��� Picker ������Ʈ
	const CPicker* m_pPlayerPicker = nullptr;

	// 1��Ī���� ������ �޽�
	CDynamicMesh* m_pFirstPersonModelCom = nullptr;
	// ���� ����
	WEAPON_TYPE m_eWeaponType;
	// ���� �̸�
	_tchar* m_szWaeponName = nullptr;
	// ������
	_float m_fDamage = 0.f;
	// �� źâ�� �ִ� ź��
	_int m_iMaxOneMagazine = 0;
	// ���� źâ�� ���� ź��
	_int m_iCurrentMagazine = 0;
	// �ִ�� ������ �ִ� ź�� ��
	_int m_iMaxAmmo = 0;
	// ���� �����ִ� ź�� ��
	_int m_iCurrentAmmo = 0;
	// �߻� �ӵ�
	_float m_fFiringRate = 0.f;
	// �߻�ӵ� üũ�� ���� Ÿ�̸�
	CTimer* m_pFiringTimer = nullptr;
	
	// �߻� �ð� ������ ���� ����
	_double m_dAccFiringTime = 0.0;

	// �ִϸ��̼� �ð� ������ ���� ����
	_double m_dAnimationTime = 0.0;

	// 1��Ī �������ݽ� �����ִ� Į
	CStaticMesh* m_pFirstPersonKnifeCom = nullptr;
	// Į�� ����ִ� ���� ���� ��ȯ���
	_matrix* m_pKnifeFrameUpdateMatrix = nullptr;
	// ���� ���°� Į�� ���̴� �������� Ȯ���ϴ� ����
	_bool m_bIsMeleeAttack = false;

	// 1��Ī �߰����ð������ ���̴� �߰����ð� �޽�
	CStaticMesh* m_pFirstPersonNVGCom = nullptr;
	// �߰����ð��� �� ��ȯ ���
	_matrix* m_pNVGFrameUpdateMatrix = nullptr;
	// ���� ���°� �߰����ð��� ���̴� �������� Ȯ��
	_bool m_bIsNVG = false;

	// ����ź�� ������ �ִϸ��̼��� ����ִ� ���̳��� �޽�
	CDynamicMesh* m_pVeiwmodelGrenade = nullptr;
	// ����ź�� ������ �������� Ȯ���ϴ� ����
	_bool m_bIsGrenade = false;

	// 3��Ī���� ������ �޽�
	CDynamicMesh* m_pThirdPersonModelCom = nullptr;
	// ���� ������� �ϴ� ���°� 3��Ī���� Ȯ���ϴ� ����
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
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	_uint m_uiShaderPass = 0;

private:
	HRESULT FirstPersonModelRender();
	HRESULT FirstPersonKnifeRender();
	//HRESULT FirstPersonNVGRender();
	
private:
	// ���̴��� ����
	HRESULT SettingShader();
	HRESULT SettingKnifeShader();
	//HRESULT SettingNVGShader();

public:
	virtual void Free();
};

END

#endif // !__WEAPON_H__