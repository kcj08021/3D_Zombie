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
	// �÷��̾��� ���� �÷��� ����ü
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
	// ������
	explicit CPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CPlayer(const CPlayer& _Player);
	// �Ҹ���
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
	// �÷��̾� ���� �÷���
	_byte m_bytePlayerFlag = 0x00;
	// �÷��̾� ü��
	_float m_fPlayerHP = c_fPlayerFullHP;
	// ȸ�� �ð� ����
	_double m_dPlayerHealingTimeAcc = 0.0;

	// ���� ������Ʈ �迭(2�� ��������)
	CWeapon* m_pWeapon[WEAPON_END];
	// ���� ���õ� ������ �ε���
	WEAPON_SELECT m_eWeaponType;

	// �������� �ݶ��̴� ������Ʈ
	CCollider* m_pMeleepColliderCom = nullptr;
	// �ǰ� �ݶ��̴� ������Ʈ
	CCollider* m_pHitColliderCom = nullptr;

	// ���� ������Ʈ �ݶ��̴� ������Ʈ
	CCollider* m_pWeaponObjectColliderCom = nullptr;
	// ���� ������Ʈ �ݶ��̴��� ���� ���� ���
	_matrix m_tWeaponObjectColliderMatrix;

	// �׺���̼� ������Ʈ
	CNavigation* m_pNavigationCom = nullptr;
	// ������ ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// ��Ŀ ������Ʈ
	CPicker* m_pPickerCom = nullptr;

	// �߷°��ӵ� ������
	_float m_fGravityAcc = 0.f;

	// ���� �����ӿ��� �̵��� �Ÿ��� ����
	_vec3 m_vPrePos;

	// ���� ���� �ð�
	_double m_dJumpTime = 0.f;

	// �ȱ� ���� �ð�
	_double m_dWalkTime = 0.f;
	// �̵��� �����ߴ��� Ȯ��
	_bool m_bIsWalk;

	/* �÷��̾� �ǰݽ� �̿�Ǵ� ������ */
	// �ǰݽ� ��鸮�� ī�޶��� ���
	_matrix m_matShakeCamera;
	// ī�޶� ��鸮�� ������ �����ϴ� ����
	_float m_fPower;
	// ī�޶� ��鸲�� ���ۉ���� üũ
	_bool m_bIsCameraShaking = false;
	// ī�޶� ��鸲�� ������ ���󺹱� ������ üũ
	_bool m_bIsCameraShakingEnd = false;
	// ī�޶� ��鸲 �ð� ���� ����
	_double m_dCameraShakingTimeAcc = 0.0;

	// �÷��̾� ���� üũ
	_bool m_bIsDead = false;

private:
	// ���۽� �����Ǵ� ���⸦ ����
	HRESULT WeaponSetting();
	// �÷��̾� ����
	void PlayerControl(_double _dDeltaTime);
	// ����
	void Jump(_double _dDeltaTime);
	// �÷��̾ ���� �������� ���
	void LandingGround(_double _dDeltaTime);
	// ī�޶� ����
	void HitCamera(_double _dDeltaTime, _float _fPower);
	// �÷��̾��� ���� ����
	void SetDead();


public:
	// ���� ���� �Լ�(static)
	static CPlayer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};

END
#endif // !__PLAYER_H__
