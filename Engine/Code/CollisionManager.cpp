#include "..\Header\CollisionManager.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager(){
}

HRESULT CCollisionManager::RenderCollider(){
	for(_uint i = 0; i < COLL_END; ++i)
		for(auto& elem : m_ColliderList[i])
			elem->RenderCollider();

	return NOERROR;
}

HRESULT CCollisionManager::AddCollider(CCollider * _Collider){
	if(nullptr == _Collider)
		return E_FAIL;
	COLL_TYPE eType;

	switch(_Collider->m_eType){
	case CCollider::COLLTYPE_AABB:
	case CCollider::COLLTYPE_OBB:
		eType = COLL_BB;
		break;
	case CCollider::COLLTYPE_SPHERE:
		eType = COLL_SPHERE;
		break;
	default:
		break;
	}

	COLL_LIST::iterator find_iter = find_if(m_ColliderList[eType].begin(), m_ColliderList[eType].end(), IsEqual_Address(_Collider));

	if(m_ColliderList[eType].end() != find_iter)
		return E_FAIL;

	m_ColliderList[eType].push_back(_Collider);
	SafeAddRef(_Collider);

	return NOERROR;
}

HRESULT CCollisionManager::RemoveCollider(CCollider * _Collider){
	if(nullptr == _Collider)
		return E_FAIL;

	COLL_TYPE eType;

	switch(_Collider->m_eType){
	case CCollider::COLLTYPE_AABB:
	case CCollider::COLLTYPE_OBB:
		eType = COLL_BB;
		break;
	case CCollider::COLLTYPE_SPHERE:
		eType = COLL_SPHERE;
		break;
	default:
		break;
	}

	COLL_LIST::iterator find_iter = find_if(m_ColliderList[eType].begin(), m_ColliderList[eType].end(), IsEqual_Address(_Collider));

	if(m_ColliderList[eType].end() == find_iter)
		return E_FAIL;

	m_ColliderList[eType].erase(find_iter);
	SafeRelease(_Collider);

	return NOERROR;
}

_bool CCollisionManager::IsCollision(CCollider * _Collider){
	if(nullptr == _Collider)
		return false;

	_bool bReturnValue = false, bTempValue = false;

	switch(_Collider->m_eType){
	case CCollider::COLLTYPE_AABB:
	case CCollider::COLLTYPE_OBB:
		for(auto& elem : m_ColliderList[COLL_BB]){
			if(elem == _Collider)
				continue;
			if(_Collider->m_eType == CCollider::COLLTYPE_AABB
				&& elem->m_eType == CCollider::COLLTYPE_AABB)
				bTempValue = IsCollision_AABB(_Collider, elem);
			else
				bTempValue = IsCollision_OBB(_Collider, elem);
			bReturnValue = bReturnValue ? bReturnValue : bTempValue;
		}
		break;
	case CCollider::COLLTYPE_SPHERE:
		for(auto& elem : m_ColliderList[COLL_SPHERE]){
			if(elem == _Collider)
				continue;

			bTempValue = IsCollision_Sphere(_Collider, elem);
			bReturnValue = bReturnValue ? bReturnValue : bTempValue;
		}
		break;
	default:
		break;
	}

	return bReturnValue;
}

_bool CCollisionManager::IsCollision_AABB(CCollider * _pSrc, CCollider * _pDest){
	if(nullptr == _pSrc
		|| nullptr == _pDest)
		return false;

	_pDest->m_bIsCollision = _pDest->m_bIsCollision ? true : false;

	_vec3 vSrcMin = _pSrc->m_vMin_AABB;
	_vec3 vSrcMax = _pSrc->m_vMax_AABB;

	_vec3 vDestMin = _pDest->m_vMin_AABB;
	_vec3 vDestMax = _pDest->m_vMax_AABB;

	D3DXVec3TransformCoord(&vSrcMin, &vSrcMin, &_pSrc->m_tWorldMatrix);
	D3DXVec3TransformCoord(&vSrcMax, &vSrcMax, &_pSrc->m_tWorldMatrix);

	D3DXVec3TransformCoord(&vDestMin, &vDestMin, &_pDest->m_tWorldMatrix);
	D3DXVec3TransformCoord(&vDestMax, &vDestMax, &_pDest->m_tWorldMatrix);

	if(max(vSrcMin.x, vDestMin.x) > min(vSrcMax.x, vDestMax.x))
		return false;

	if(max(vSrcMin.y, vDestMin.y) > min(vSrcMax.y, vDestMax.y))
		return false;

	if(max(vSrcMin.z, vDestMin.z) > min(vSrcMax.z, vDestMax.z))
		return false;

	_pSrc->m_pCollisionObject = _pDest->m_tColliderDesc.pGameObj;
	_pDest->m_pCollisionObject = _pSrc->m_tColliderDesc.pGameObj;

	_pSrc->m_bIsCollision = true;
	_pDest->m_bIsCollision = true;

	return true;
}

_bool CCollisionManager::IsCollision_OBB(CCollider * _pSrc, CCollider * _pDest){
	// 두 콜라이더의 OBB 정보
	CCollider::OBB_DESC tOBB_Desc[2] = {
		*_pSrc->m_pOBB_Desc, *_pDest->m_pOBB_Desc
	};
	// 두 콜라이더의 월드 변환 행렬
	_matrix tWolrdMatrix[2] = {
		_pSrc->m_tWorldMatrix, _pDest->m_tWorldMatrix
	};
	
	// 두 콜라이더가 가진 OBB정보를 월드 변환
	for(_uint i = 0; i < 2; ++i){
		D3DXVec3TransformCoord(&tOBB_Desc[i].vCenterPos, &tOBB_Desc[i].vCenterPos, &tWolrdMatrix[i]);
			for(_uint j = 0; j < 3; ++j){
				D3DXVec3TransformNormal(&tOBB_Desc[i].vProjAxis[j], &tOBB_Desc[i].vProjAxis[j], &tWolrdMatrix[i]);
				D3DXVec3TransformNormal(&tOBB_Desc[i].vAlignAxis[j], &tOBB_Desc[i].vAlignAxis[j], &tWolrdMatrix[i]);
				D3DXVec3Normalize(&tOBB_Desc[i].vAlignAxis[j], &tOBB_Desc[i].vAlignAxis[j]);
			}
	}

	_float fDistance[3] = {};

	_pDest->m_bIsCollision = _pDest->m_bIsCollision ? true : false;
	
	_vec3 vDir = tOBB_Desc[1].vCenterPos - tOBB_Desc[0].vCenterPos;
	for(_uint i = 0; i < 2; ++i){
		for(_uint j = 0; j < 3; ++j){
			fDistance[0] = fabs(D3DXVec3Dot(&vDir, &tOBB_Desc[i].vAlignAxis[j]));
			
			fDistance[1] = fabs(D3DXVec3Dot(&tOBB_Desc[0].vProjAxis[0], &tOBB_Desc[i].vAlignAxis[j]))
				+ fabs(D3DXVec3Dot(&tOBB_Desc[0].vProjAxis[1], &tOBB_Desc[i].vAlignAxis[j]))
				+ fabs(D3DXVec3Dot(&tOBB_Desc[0].vProjAxis[2], &tOBB_Desc[i].vAlignAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&tOBB_Desc[1].vProjAxis[0], &tOBB_Desc[i].vAlignAxis[j]))
				+ fabs(D3DXVec3Dot(&tOBB_Desc[1].vProjAxis[1], &tOBB_Desc[i].vAlignAxis[j]))
				+ fabs(D3DXVec3Dot(&tOBB_Desc[1].vProjAxis[2], &tOBB_Desc[i].vAlignAxis[j]));
		
			if(fDistance[1] + fDistance[2] < fDistance[0])
				return false;
		}
	}
	
	_pSrc->m_bIsCollision = true;
	_pDest->m_bIsCollision = true;

	_pSrc->m_pCollisionObject = _pDest->m_tColliderDesc.pGameObj;
	_pDest->m_pCollisionObject = _pSrc->m_tColliderDesc.pGameObj;

	return true;
}

_bool CCollisionManager::IsCollision_Sphere(CCollider * _pSrc, CCollider * _pDest){
	_vec3 vSrcPos(_pSrc->m_tWorldMatrix.m[3]);
	_vec3 vDestPos(_pDest->m_tWorldMatrix.m[3]);

	_float fAbsDest = (_pSrc->m_tColliderDesc.vScale.x + _pDest->m_tColliderDesc.vScale.x) * 0.5f;
	_float fDist = D3DXVec3Length(&(vDestPos - vSrcPos));

	if(fAbsDest < fDist)
		return false;

	_pSrc->m_bIsCollision = true;
	_pDest->m_bIsCollision = true;

	_pSrc->m_pCollisionObject = _pDest->m_tColliderDesc.pGameObj;
	_pDest->m_pCollisionObject = _pSrc->m_tColliderDesc.pGameObj;

	return true;
}


void CCollisionManager::Free(){
	for(_uint i = 0; i < COLL_END; ++i){
		for(auto& elem : m_ColliderList[i])
			SafeRelease(elem);
		m_ColliderList[i].clear();
	}
}