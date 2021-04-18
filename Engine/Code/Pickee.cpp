#include "..\Header\Pickee.h"
#include "..\Header\PickingManager.h"
#include "..\Header\Pipeline.h"

// ������
CPickee::CPickee(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice){
	::ZeroMemory(&m_tPickeeDesc, sizeof(PICKEE_DESC));
}

// ���� ������
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
		// ���� �ʱ�ȭ�� ���� �������� �޾ƿ�
		::memcpy(&m_tPickeeDesc, _pArgument, sizeof(PICKEE_DESC));

		CPickingManager* pPickingManager = CPickingManager::GetInstance();

		SafeAddRef(pPickingManager);

		// ��ŷ �Ŵ����� ��ŷ ����� ���μ����� ����
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

	// ��ŷ �Ŵ����� ��ŷ ����� ���μ����� ����
	pPickingManager->ChangePickeeDesc(&m_tPickeeDesc);

	SafeRelease(pPickingManager);

	return NOERROR;
}

HRESULT CPickee::RemovePickeeDesc(){
	CPickingManager* pPickingManager = CPickingManager::GetInstance();
	if(nullptr == pPickingManager)
		return E_FAIL;

	SafeAddRef(pPickingManager);

	// ��ŷ �Ŵ����� ��ŷ ����� ���μ����� ����
	pPickingManager->RemovePickeeDesc(&m_tPickeeDesc);

	SafeRelease(pPickingManager);

	return NOERROR;
}



// ���� ���� �Լ�(static)
CPickee * CPickee::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CPickee* pInstance = new CPickee(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CPickee Create Failure");
	}
	return pInstance;
}

// ���� ���� �Լ�
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
