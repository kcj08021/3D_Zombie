#ifndef __M16M203_H__
#define __M16M203_H__


#include "Weapon.h"

BEGIN(Client)

class Cm16m203 final:
	public CWeapon{
private:
	enum ANIMATION_SET{
		IDLE = 0,
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
		SOUND_CHAMBER1,
		SOUND_CHAMBER2,
		SOUND_CLIP_IN,
		SOUND_CLIP_OUT,
		SOUND_LIFT,
		SOUND_SET_END
	};


private:
	// 생성자
	explicit Cm16m203(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit Cm16m203(const Cm16m203& _m16m203);
	// 소멸자
	virtual ~Cm16m203() = default;

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
	// 조작
	virtual HRESULT WeaponControl(CPicker* _pPicker);
	// 애니메이션 업데이트
	virtual HRESULT UpdateWeaponState(_double _dDeltaTime);

private:
	_bool m_bAnimationSound[SOUND_SET_END];

public:
	// CWeapon을(를) 통해 상속됨
	virtual HRESULT ResetAnimationSound() override;

public:
	// 원본 생성 함수(static)
	static Cm16m203* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();

};

END

#endif // !__M16M203_H__

