#include "stdafx.h"

#include "..\Header\Client_Headers.h"
#include "..\Header\BloodEffect.h"


USING(Client)

CBloodEffect::CBloodEffect(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CEffect(_pGraphicDevice){
}

CBloodEffect::CBloodEffect(const CBloodEffect& _MuzzleEffect)
	: CEffect(_MuzzleEffect){
}

// Initialize with Prototype
HRESULT CBloodEffect::InitGameObject_Proto(){
	return CEffect::InitGameObject_Proto();
}

// Initialize with Clone
HRESULT CBloodEffect::InitGameObject_Clone(void * _pArgument){
	return CEffect::InitGameObject_Clone(_pArgument);
}

// LateInitialize
HRESULT CBloodEffect::LateInitGameObject(){
	return CEffect::LateInitGameObject();
}

// Update
HRESULT CBloodEffect::UpdateGameObject(_double _dDeltaTime){
	return CEffect::UpdateGameObject(_dDeltaTime);
}

// LateUpdate
HRESULT CBloodEffect::LateUpdateGameObject(_double _dDeltaTime){

	return CEffect::LateUpdateGameObject(_dDeltaTime);
}

// Render
HRESULT CBloodEffect::RenderGameObject(_double _dDeltaTime){
	return CEffect::RenderGameObject(_dDeltaTime);
}

// 원본 생성 함수(static)
CBloodEffect * CBloodEffect::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CBloodEffect* pInstance = new CBloodEffect(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CBloodEffect Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject* CBloodEffect::CreateClone(void * _pArgument){
	CBloodEffect* pInstance = new CBloodEffect(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CBloodEffect Clone Failure");
	}

	return pInstance;
}

void CBloodEffect::Free(){


	CEffect::Free();
}

