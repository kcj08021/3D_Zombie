#ifndef __M16M203_GRENADE_H__
#define __M16M203_GRENADE_H__

#include "..\Header\Weapon.h"

BEGIN(Client)

class CM16M203_Grenade:
	public CWeapon{
private:
	enum ANIMATION_SET{
		IDLE = 0,
		FIRE,					// = 1
		RELOAD,					// = 2
		SPRINT_IN,				// = 3
		SPRINT_LOOP,			// = 4
		SPRINT_OUT,				// = 5
		GRENADE_TO_BULLET,		// = 6
		BULLET_TO_GRENADE,		// = 7
		PULLOUT,				// = 8
		PUTAWAY,				// = 9
		QUICK_PULLOUT,			// = 10
		QUICK_PUTAWAY,			// = 11
		KNIFE_MELEE1,			// = 12
		KNIFE_MELEE2,			// = 13
		NVG_PUTON,				// = 14
		NVG_TAKEOFF,			// = 15
	};

private:
	// 생성자
	explicit CM16M203_Grenade(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CM16M203_Grenade(const CM16M203_Grenade& _m4m203);
	// 소멸자
	virtual ~CM16M203_Grenade() = default;

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
	static CM16M203_Grenade* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();

	// CWeapon을(를) 통해 상속됨
	virtual HRESULT ResetAnimationSound() override;
};

END

#endif // !__M16M203_GRENADE_H__
