#include "..\Header\Pickee.h"
#include "..\Header\PickingManager.h"
#include "..\Header\Pipeline.h"

// 생성자
CPickee::CPickee(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice){
	::ZeroMemory(&m_tPickeeDesc, sizeof(PICKEE_DESC));
}

// 복사 생성자
CPickee::CPickee(const CPickee & _Pickee)
	: CComponent(_Pickee)
	, m_tPickeeDesc(_Pickee.m_tPickeeDesc){
}


// Initialize with Prototype
HRESULT CPickee::InitComponent_Proto(){
	return NOERROR;
}

// Initialize with Clone
HRESULT CPickee::InitComponent_Clone(void* _pArgument){
	if(nullptr != _pArgument){
		// 복제 초기화시 세부 설정값을 받아옴
		::memcpy(&m_tPickeeDesc, _pArgument, sizeof(PICKEE_DESC));

		CPickingManager* pPickingManager = CPickingManager::GetInstance();

		SafeAddRef(pPickingManager);

		// 피킹 매니저에 피킹 대상의 세부설정을 전달
		pPickingManager->AddPickee(&m_tPickeeDesc);

		SafeRelease(pPickingManager);
	}

	return NOERROR;
}

HRESULT CPickee::ChangePickeeDesc(PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	::memcpy(&m_tPickeeDesc, _pPickeeDesc, sizeof(PICKEE_DESC));

	CPickingManager* pPickingManager = CPickingManager::GetInstance();

	SafeAddRef(pPickingManager);

	// 피킹 매니저에 피킹 대상의 세부설정을 전달
	pPickingManager->ChangePickeeDesc(&m_tPickeeDesc);

	SafeRelease(pPickingManager);

	return NOERROR;
}

HRESULT CPickee::RemovePickeeDesc(){
	CPickingManager* pPickingManager = CPickingManager::GetInstance();
	if(nullptr == pPickingManager)
		return E_FAIL;

	SafeAddRef(pPickingManager);

	// 피킹 매니저에 피킹 대상의 세부설정을 전달
	pPickingManager->RemovePickeeDesc(&m_tPickeeDesc);

	SafeRelease(pPickingManager);

	return NOERROR;
}



// 원본 생성 함수(static)
CPickee * CPickee::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CPickee* pInstance = new CPickee(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CPickee Create Failure");
	}
	return pInstance;
}

// 복제 생성 함수
CComponent * CPickee::CreateClone(void* _pArgument){
	CPickee* pInstance = new CPickee(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CPickee Clone Failure");
	}
	return pInstance;
}

// Free
void CPickee::Free(){
	CComponent::Free();
}
