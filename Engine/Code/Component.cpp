#include "..\Header\Component.h"

// 持失切
CComponent::CComponent(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: m_pGraphicDevice(_pGraphicDevice)
	, m_bIsClone(false){
	SafeAddRef(_pGraphicDevice);
}

// 差紫 持失切
CComponent::CComponent(const CComponent & _Component)
	: m_pGraphicDevice(_Component.m_pGraphicDevice)
	, m_bIsClone(true){
	SafeAddRef(_Component.m_pGraphicDevice);
}

// Initialize with Prototype
HRESULT CComponent::InitComponent_Proto(){
	return NOERROR;
}

// Free
void CComponent::Free(){
	SafeRelease(m_pGraphicDevice);
}

