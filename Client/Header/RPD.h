#ifndef __RPD_H__
#define __RPD_H__

#include "Weapon.h"

BEGIN(Client)

class CRPD:
	public CWeapon{
private:
	enum ANIMATION_SET{
		IDLE				   = 0,
		FIRE,				// = 1
		RELOAD,				// = 2
		SPRINT_IN,			// = 3
		SPRINT_LOOP,		// = 4
		SPRINT_OUT,			// = 5
		PULLOUT,			// = 6
		PUTAWAY,			// = 7
		KNIFE_MELEE1,		// = 8
		KNIFE_MELEE2,		// = 9
		NVG_PUTON,			// = 10
		NVG_TAKEOFF,		// = 11
		ADS_UP,				// = 12
		ADS_IDLE,			// = 13
		ADS_FIRE,			// = 14
		ADS_DOWN			// = 15
	};
	enum ANIMATION_SOUND_SET{
		SOUND_CHAMBER,
		SOUND_OPEN,
		SOUND_CLOSE,
		SOUND_HIT,
		SOUND_CLIP_IN,
		SOUND_CLIP_OUT,
		SOUND_LIFT,
		SOUND_SET_END
	};

private:
	// 생성자
	explicit CRPD(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사생성자
	explicit CRPD(const CRPD& _ak47);
	// 소멸자
	virtual ~CRPD() = default;

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

private:
	_bool m_bAnimationSound[SOUND_SET_END];

public:
	// 원본 생성 함수(static)
	static CRPD* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* pArgument);

public:
	// Free
	virtual void Free();

	// CWeapon을(를) 통해 상속됨
	virtual HRESULT ResetAnimationSound() override;
};

END

#endif // !__RPD_H__
