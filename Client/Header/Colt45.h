#ifndef __COLT45_H__
#define __COLT45_H__


#include "Weapon.h"

BEGIN(Client)

class CColt45:
	public CWeapon{
private:
	enum ANIMATION_SET{
		IDLE = 0,
		FIRE,				// = 1
		RELOAD,				// = 2
		SPRINT_IN,			// = 3
		SPRINT_LOOP,		// = 4
		SPRINT_OUT,			// = 5
		FIRST_TIME_PULLOUT,	// = 6
		PULLOUT,			// = 7
		PUTAWAY,			// = 8
		KNIFE_MELEE1,		// = 9
		KNIFE_MELEE2,		// = 10
		NVG_PUTON,			// = 11
		NVG_TAKEOFF,		// = 12
		ADS_UP,				// = 13
		ADS_IDLE,			// = 14
		ADS_FIRE,			// = 15
		ADS_DOWN,			// = 16
		LASTFIRE,			// = 17
		IDLE_EMPTY,			// = 18
		SPRINT_IN_EMPTY,	// = 19
		SPRINT_LOOP_EMPTY,	// = 20
		SPRINT_OUT_EMPTY,	// = 21
		PULLOUT_EMPTY,		// = 22
		PUTAWAY_EMPTY,		// = 23
		NVG_PUTON_EMPTY,	// = 24
		NVG_TAKEOFF_EMPTY,	// = 25
		ADS_UP_EMPTY,		// = 26
		ADS_LASTFIRE,		// = 27
		ADS_IDLE_EMPTY,		// = 28
		ADS_DOWN_EMPTY,		// = 29
		RELOAD_EMPTY		// = 30
	};

private:
	// 생성자
	explicit CColt45(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CColt45(const CColt45& _Beretta);
	// 소멸자
	virtual ~CColt45() = default;

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


public:
	// 원본 생성 함수(static)
	static CColt45* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};

END

#endif // !__COLT45_H__
