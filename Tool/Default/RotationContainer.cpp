#include "stdafx.h"
#include "..\Header\RotationContainer.h"

USING(Tool)

CRotationContainer::CRotationContainer(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice){
	::ZeroMemory(m_RotationValue, sizeof(_float) * 3);
}

CRotationContainer::CRotationContainer(const CRotationContainer & _RotationContainer)
	: CComponent(_RotationContainer){
	::memcpy(m_RotationValue, _RotationContainer.m_RotationValue, sizeof(_float) * 3);
}

HRESULT CRotationContainer::InitComponent_Proto(){
	return NOERROR;
}

HRESULT CRotationContainer::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

void CRotationContainer::SetRotationValue(const _float *& _fRotationValue){
	if(nullptr == _fRotationValue)
		return;

	::memcpy(m_RotationValue, _fRotationValue, sizeof(_float) * 3 );
}

void CRotationContainer::SetRotationAxis(_uint _uiAxis, _float fRotate){
	if(3 <= _uiAxis)
		return;

	m_RotationValue[_uiAxis] = fRotate;
}

void CRotationContainer::GetRotationValue(_float *& _fRotationValue){
	if(nullptr == _fRotationValue)
		return;

	::memcpy(_fRotationValue, m_RotationValue, sizeof(_float) * 3);
}

_float CRotationContainer::GetRotationAxis(_uint _uiAxis){
	if(3 <= _uiAxis)
		return 0.f;

	return m_RotationValue[_uiAxis];
}

CRotationContainer * CRotationContainer::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CRotationContainer* pInstance = new CRotationContainer(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CRotationContainer Create Failure");
	}
	
	return pInstance;
}

CComponent * CRotationContainer::CreateClone(void * _pArgument){
	CRotationContainer* pInstance = new CRotationContainer(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CRotationContainer Clone Failure");
	}

	return pInstance;
}

void CRotationContainer::Free(){
	CComponent::Free();
}
