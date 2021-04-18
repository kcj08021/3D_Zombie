#ifndef __WEAPON_OBJECT_H__
#define __WEAPON_OBJECT_H__

#include "GameObject.h"

BEGIN(Client)
class CWeapon;

class CWeaponObject final:
	public CGameObject{
public:
	typedef struct tagWeaponObjectDesc{
		CWeapon* pWeapon;
		_matrix* pWorldMatrix;
	}WEAPONOBJECT_DESC;

private:
	explicit CWeaponObject(LPDIRECT3DDEVICE9 _pGraphicDeive);
	explicit CWeaponObject(const CWeaponObject& _WeaponObject);
	virtual ~CWeaponObject() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(CStaticMesh* _pStaticMeshStageMap);
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) override;
public:
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

public:
	void SetStageMap(CStaticMesh* _pStaticMesh){
		m_pStaticMeshStageMap = _pStaticMesh;
		SafeAddRef(m_pStaticMeshStageMap);
	}
	HRESULT ChangeWeapon(CWeapon** _pWeapon);
	const _tchar* GetName();


private:
	CTransform* m_pTransformCom = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShaderCom = nullptr;
	CCollider* m_pCollider = nullptr;
	CWeapon* m_pWeapon = nullptr;

	CStaticMesh* m_pStaticMeshStageMap = nullptr;
	_float m_fGravityAcc = 0.f;
	_bool m_bIsGround = false;
	_float m_fDeadTime = 0.f;

private:
	HRESULT SettingShader();
	void LandingGround(_double _dDeltaTime);

public:
	static CWeaponObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CStaticMesh* _pStaticMeshStageMap);
	virtual CGameObject* CreateClone(void* _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__WEAPON_OBJECT_H__

