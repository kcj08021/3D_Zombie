#include "stdafx.h"
#include "..\Header\UnitManager.h"
#include "..\Header\NonPlayer.h"

#include "..\Header\Client_Headers.h"

#include "Management.h"

USING(Client)

IMPLEMENT_SINGLETON(CUnitManager)

CUnitManager::CUnitManager()
	: m_pResponedTime(0.0)
	, m_uiCurrentSceanNum(0)
	, m_pStageMap(nullptr)
	, m_pPlayerPos(nullptr)
	, m_pPlayerWeapon(nullptr)
	, m_pPlayerHP(nullptr)
	, m_pGroundWeaponName(nullptr)
	, m_bIsCollisionGroundWeapon(false)
	, m_fDamage(0.f){
}

void CUnitManager::SetResponPos(_vec3 & _vPos){
	m_ListResponPos.push_back(_vPos);
}

_bool CUnitManager::IntersectRayToStageMap(const _vec3& _vDir, const _vec3& vPos, _float* _fOutDist/* = nullptr*/){
	if(nullptr == m_pStageMap)
		return false;

	BOOL bHit = false;
	DWORD dwFaceIndex = 0;
	_float fU, fV, fDist;
	LPD3DXBUFFER pAllHit = nullptr;
	DWORD dwAllHitCount = 0;

	if(FAILED(D3DXIntersect(m_pStageMap->GetMesh(), &vPos, &_vDir, &bHit, &dwFaceIndex, &fU, &fV, &fDist, &pAllHit, &dwAllHitCount)))
		return false;

	if(nullptr != _fOutDist
		&& TRUE == bHit)
		*_fOutDist = fDist;

	return TRUE == bHit;
}

HRESULT CUnitManager::ResponNonPlayer(_double _dDeltaTime){
	m_pResponedTime += _dDeltaTime;
	if(m_ListResponPos.size() && m_pResponedTime > 10.0){

		CManagement* pManagement = CManagement::GetInstance();
		if(nullptr == pManagement)
			return E_FAIL;
		SafeAddRef(pManagement);

		CNonPlayer::NONPLAYER_DESC tDesc = {};
		tDesc.szDynamicMeshProtoName = L"zombie";
		tDesc.tTransformDesc.fRotationPerSec = 0;
		tDesc.tTransformDesc.fSpeedPerSec = 1.2f;
		_matrix matWorld;

		for(auto elem : m_ListResponPos){
			auto EnemyList = pManagement->GetObjectList(STAGE_SCENE, L"EnemyLayer");
			if(nullptr != EnemyList){
				if(EnemyList->size() > 30){
					SafeRelease(pManagement);
					return NOERROR;
				}
			}
			D3DXMatrixTranslation(&matWorld, elem.x, elem.y, elem.z);
			tDesc.tTransformDesc.pMatrixWorldSetting = &matWorld;

			if(FAILED(pManagement->AddCloneObjectToLayer(L"3rd_Model_Proto", m_uiCurrentSceanNum, L"EnemyLayer", &tDesc))){
				SafeRelease(pManagement);
				return E_FAIL;
			}
		}

		SafeRelease(pManagement);

		m_pResponedTime = 0.0;
	}

	return NOERROR;
}

void CUnitManager::Free(){
	SafeRelease(m_pStageMap);
}
