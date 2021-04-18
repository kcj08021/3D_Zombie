#ifndef __MUZZLE_EFFECT_H__
#define __MUZZLE_EFFECT_H__

#include "Effect.h"

BEGIN(Client)

class CMuzzleEffect:
	public CEffect{
public:
	explicit CMuzzleEffect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CMuzzleEffect(const CMuzzleEffect& _MuzzleEffect);
	virtual ~CMuzzleEffect() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
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
	// 원본 생성 함수(static)
	static CMuzzleEffect* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free() override;
};

END

#endif // !__MUZZLE_EFFECT_H__
