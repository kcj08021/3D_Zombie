#ifndef __GRENADE_H__
#define __GRENADE_H__

#include "GameObject.h"

BEGIN(Client)

class CGrenade final
	: public CGameObject{
public:
	enum GRENADE_TYPE{
		HAND, FLASH = 0,LAUNCHER
	};

	typedef struct tagGrenade{
		_matrix matWorld;
		GRENADE_TYPE eType;
	}GRENADE_DESC;
private:
	explicit CGrenade(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CGrenade(const CGrenade& _Grenade);
	virtual ~CGrenade();

private:
	virtual HRESULT InitGameObject_Proto();
	virtual HRESULT InitGameObject_Clone(void * _pArgument) override;

public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

private:
	GRENADE_TYPE m_eType;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CPicker* m_pPicker = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CStaticMesh* m_pGrenadeMeshCom = nullptr;
	CFrustum* m_pFrustum = nullptr; 
	_float m_fSpeed = 0.f;
	_float m_fRange = 0.f;
	_vec3 m_vDir;
	_bool m_bIsHit;

private:
	HRESULT SetExplosion();

public:
	static CGrenade* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual CGameObject * CreateClone(void * _pArgument) override;
	virtual void Free();
};

END

#endif // !__GRENADE_H__

