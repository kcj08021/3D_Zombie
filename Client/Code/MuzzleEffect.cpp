#include "stdafx.h"

#include "..\Header\Client_Headers.h"
#include "..\Header\MuzzleEffect.h"

USING(Client)

CMuzzleEffect::CMuzzleEffect(LPDIRECT3DDEVICE9 _pGraphicDevice) 
	: CEffect(_pGraphicDevice){
}

CMuzzleEffect::CMuzzleEffect(const CMuzzleEffect & _MuzzleEffect)
	: CEffect(_MuzzleEffect){
}

// Initialize with Prototype
HRESULT CMuzzleEffect::InitGameObject_Proto(){
	return CEffect::InitGameObject_Proto();
}

// Initialize with Clone
HRESULT CMuzzleEffect::InitGameObject_Clone(void * _pArgument){
	return CEffect::InitGameObject_Clone(_pArgument);
}

// LateInitialize
HRESULT CMuzzleEffect::LateInitGameObject(){
	return CEffect::LateInitGameObject();
}

// Update
HRESULT CMuzzleEffect::UpdateGameObject(_double _dDeltaTime){
	return CEffect::UpdateGameObject(_dDeltaTime);
}

// LateUpdate
HRESULT CMuzzleEffect::LateUpdateGameObject(_double _dDeltaTime){
	return CEffect::LateUpdateGameObject(_dDeltaTime);
}

// Render
HRESULT CMuzzleEffect::RenderGameObject(_double _dDeltaTime){
	return CEffect::RenderGameObject(_dDeltaTime);
}

// 원본 생성 함수(static)
CMuzzleEffect * CMuzzleEffect::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CMuzzleEffect* pInstance = new CMuzzleEffect(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CMuzzleEffect Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject* CMuzzleEffect::CreateClone(void * _pArgument){
	CMuzzleEffect* pInstance = new CMuzzleEffect(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CMuzzleEffect Clone Failure");
	}

	return pInstance;
}

void CMuzzleEffect::Free(){


	CEffect::Free();
}

