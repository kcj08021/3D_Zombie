#ifndef __CAK47_H__
#define __CAK47_H__
#include "Weapon.h"

BEGIN(Client)

class Cak47:
	public CWeapon{
private:
	enum ANIMATION_SET{
		IDLE				   = 0,
		FIRE,				// = 1
		RELOAD,				// = 2
		RELOAD_EMPTY,		// = 3
		SPRINT_IN,			// = 4
		SPRINT_LOOP,		// = 5
		SPRINT_OUT,			// = 6
		FIRST_TIME_PULLOUT,	// = 7
		PULLOUT,			// = 8
		PUTAWAY,			// = 9
		QUICK_PULLOUT,		// = 10
		QUICK_PUTAWAY,		// = 11
		KNIFE_MELEE1,		// = 12
		KNIFE_MELEE2,		// = 13
		NVG_PUTON,			// = 14
		NVG_TAKEOFF,		// = 15
		ADS_UP,				// = 16
		ADS_IDLE,			// = 17
		ADS_FIRE,			// = 18
		ADS_DOWN,			// = 19		
	};
	enum ANIMATION_SOUND_SET{
		SOUND_CHAMBER,
		SOUND_CLIP_IN,
		SOUND_CLIP_OUT,
		SOUND_LIFT,
		SOUND_SET_END
	};


private:
	// ������
	explicit Cak47(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���������
	explicit Cak47(const Cak47& _ak47);
	// �Ҹ���
	virtual ~Cak47() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitWeapon();
	// Update
	virtual HRESULT UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime);
	// Render
	virtual HRESULT RenderWeapon();

private:
	virtual HRESULT WeaponControl(CPicker* _pPicker);
	virtual HRESULT UpdateWeaponState(_double _dDeltaTime);

public:
	virtual HRESULT ResetAnimationSound() override;

private:
	// �ִϸ��̼� �ð� ������ ���� ����
	_bool m_bAnimationSound[SOUND_SET_END] = {};


public:
	// ���� ���� �Լ�(static)
	static Cak47* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* pArgument);

public:
	// Free
	virtual void Free();
};

END

#endif // !__CAK47_H__