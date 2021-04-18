#include "stdafx.h"
#include "..\Header\Gravity.h"
#include "Management.h"

USING(Client)

CGravity::CGravity(LPDIRECT3DDEVICE9 _pGraphicDeivce)
	: CComponent(_pGraphicDeivce)
	, m_pTerrainStaticMesh(nullptr){
}

CGravity::CGravity(const CGravity & _Gravity)
	: CComponent(_Gravity)
	, m_pTerrainStaticMesh(_Gravity.m_pTerrainStaticMesh){
	SafeAddRef(m_pTerrainStaticMesh);
}

HRESULT CGravity::InitComponent_Proto(const CStaticMesh * _pStaticMesh){
	if(nullptr == _pStaticMesh)
		return E_FAIL;

	m_pTerrainStaticMesh = _pStaticMesh;
	SafeAddRef(_pStaticMesh);

	return NOERROR;
}

HRESULT CGravity::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

HRESULT CGravity::CheckGround(CTransform * _pTransform){
	return NOERROR;
}

CGravity * CGravity::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce, const CStaticMesh * pStaticMesh){
	CGravity* pInstance = new CGravity(_pGraphicDeivce);
	if(FAILED(pInstance->InitComponent_Proto(pStaticMesh))){
		MSG_BOX(L"CGravity Create Failure");
		SafeRelease(pInstance);
	}
	
	return pInstance;
}

CComponent * CGravity::CreateClone(void * _pArgument){
	CGravity* pInstance = new CGravity(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		MSG_BOX(L"CGravity Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CGravity::Free(){
	SafeRelease(m_pTerrainStaticMesh);

	CComponent::Free();
}
