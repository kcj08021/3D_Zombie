#ifndef __BLOOD_EFFECT_H__
#define __BLOOD_EFFECT_H__

#include "Effect.h"

BEGIN(Client)

class CBloodEffect:
	public CEffect{
public:
	explicit CBloodEffect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CBloodEffect(const CBloodEffect& _BloodEffect);
	virtual ~CBloodEffect()=default;

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
	static CBloodEffect* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free() override;
};

END

#endif // !__BLOOD_EFFECT_H__

