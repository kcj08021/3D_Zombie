#ifndef __UNIT_MANAGER_H__
#define __UNIT_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CStaticMesh;
END

BEGIN(Client)
class CWeapon;
class CUnitManager final
	: public CBase{
	DECLARE_SINGLETON(CUnitManager)
private:
	explicit CUnitManager();
	virtual ~CUnitManager() = default;

	// Get
public:
	_uint GetCurrentSceanNum(){
		return m_uiCurrentSceanNum;
	}
	CStaticMesh* GetStageMap(){
		return m_pStageMap;
	}
	_vec3 GetPlayerPos(){
		if(nullptr == m_pPlayerPos)
			return _vec3();
		return *m_pPlayerPos;
	}

	const CWeapon* GetPlayerWeapon(){
		return m_pPlayerWeapon;
	}

	_int GetPlayerHP(){
		return *m_pPlayerHP > 0.f ? static_cast<_int>(*m_pPlayerHP) : 0;
	}

	_float GetPlayerDamage(){
		_float temp = m_fDamage;
		m_fDamage = 0.f;
		return temp;
	}

	_bool IsCollisionGroundWeapon(){
		return m_bIsCollisionGroundWeapon;
	}

	const _tchar* GetGroundWeaponName(){
		return m_pGroundWeaponName;
	}


	// Set
public:
	void SetCurrentSceanNum(_uint _SceanNum){
		m_uiCurrentSceanNum = _SceanNum;
	}
	void SetStageMap(CStaticMesh* _pStaticMeshObejct){
		m_pStageMap = _pStaticMeshObejct;
		SafeAddRef(m_pStageMap);
	}
	void SetPlayerPos(_vec3* _pPlayerPos){
		m_pPlayerPos = _pPlayerPos;
	}
	void SetPlayerDamage(_float _fDamage){
		m_fDamage += _fDamage;
	}
	void SetPlayerWeapon(const CWeapon* _pPlayerWeapon){
		m_pPlayerWeapon = _pPlayerWeapon;
	}
	void SetPlayerHP(const _float* _PlayerHP){
		m_pPlayerHP = _PlayerHP;
	}
	void SetCollisionGroundWeapon(_bool _bIsCollisionGroundWeapon){
		m_bIsCollisionGroundWeapon = _bIsCollisionGroundWeapon;
	}
	void SetGroundWeaponName(const _tchar* _WeaponName){
		m_pGroundWeaponName = _WeaponName;
	}
	void SetResponPos(_vec3& _vPos);

public:
	_bool IntersectRayToStageMap(const _vec3& _vDir, const _vec3& vPos, _float* _pOutDist = nullptr);
	HRESULT ResponNonPlayer(_double _dDeltaTime);
private:
	_uint m_uiCurrentSceanNum = 0;
	CStaticMesh* m_pStageMap = nullptr;

	_vec3* m_pPlayerPos = nullptr;
	const CWeapon* m_pPlayerWeapon = nullptr;
	const _float* m_pPlayerHP = nullptr;

	const _tchar* m_pGroundWeaponName = nullptr;
	_bool m_bIsCollisionGroundWeapon = false;

	_float m_fDamage = 0.f;

	_double m_pResponedTime = 0.0;
	list<_vec3> m_ListResponPos;

public:
	virtual void Free();
};

END

#endif // !__UNIT_MANAGER_H__
