#include "..\Header\GameObject.h"
#include "..\Header\Pipeline.h"

// 생성자
CGameObject::CGameObject(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:m_pGraphicDevice(_pGraphicDevice)
	, m_bIsLateInit(false)
	, m_bIsDeadObject(false)
	, m_fViewZ(0.f)
{
	SafeAddRef(_pGraphicDevice);
}

// 복사 생성자
CGameObject::CGameObject(const CGameObject & _pObj)
	: m_pGraphicDevice(_pObj.m_pGraphicDevice)
	, m_bIsLateInit(false)
	, m_bIsDeadObject(false)
	, m_fViewZ(0.f){
	SafeAddRef(_pObj.m_pGraphicDevice);
}

// Initialize with Prototype
HRESULT CGameObject::InitGameObject_Proto(){
	return NOERROR;
}

// LateInitialize
HRESULT CGameObject::LateInitGameObject(){
	m_bIsLateInit = true;

	return this->LateInitGameObject();
}

// Update
HRESULT CGameObject::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

// LateUpdate
HRESULT CGameObject::LateUpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

// Render
HRESULT CGameObject::RenderGameObject(_double _dDeltaTime){
	return NOERROR;
}

// 컴포넌트를 복제 및 컨테이너에 보관하기 위한 함수
HRESULT CGameObject::AddComponent(CComponent** _ppOutComponent, const _tchar* _szCloneKey, const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument/* = nullptr*/){
	if(nullptr == _szProtoKey || nullptr == _szCloneKey)
		return E_FAIL;

	if(nullptr != FindComponent(_szCloneKey))
		return E_FAIL;

	CComponentManager* pComponentMgr = CComponentManager::GetInstance();

	if(nullptr == pComponentMgr)
		return E_FAIL;

	SafeAddRef(pComponentMgr);

	HRESULT hr = 0;

	CComponent* pComponent = pComponentMgr->AddCloneComponent(_szProtoKey, _uiIndex, _pArgument);
	if(nullptr != pComponent){
		m_ComponentMap[_szCloneKey] = pComponent;
		*_ppOutComponent = pComponent;
		hr = NOERROR;
	} else{
		hr = E_FAIL;
	}

	SafeRelease(pComponentMgr);
	
	return hr;
}

// 가진 특정 컴포넌트를 지우는 함수
HRESULT CGameObject::ReleaseComponent(CComponent ** _ppComponent, const _tchar * _szCloneKey){
	if(nullptr == _ppComponent
		|| nullptr == _szCloneKey)
		return E_FAIL;

	CComponent* pComponent = FindComponent(_szCloneKey);
	if(nullptr == pComponent
		|| *_ppComponent != pComponent)
		return E_FAIL;

	SafeRelease(*_ppComponent);
	m_ComponentMap.erase(_szCloneKey);

	return NOERROR;
}

HRESULT CGameObject::ComputeViewZ(_vec3 _vWorldPos){
	CPipeline* pPipeline = CPipeline::GetInstance();
	if(nullptr == pPipeline)
		return E_FAIL;

	SafeAddRef(pPipeline);

	_matrix matCameraWorld = pPipeline->GetCameraWorldMatrix();

	m_fViewZ = D3DXVec3Length(&(_vWorldPos - *reinterpret_cast<_vec3*>(matCameraWorld.m[3])));

	SafeRelease(pPipeline);

	return NOERROR;
}

// 컴포넌트 컨테이너에서 검색하는 함수
CComponent * CGameObject::FindComponent(const _tchar * _szFindString){
	if(nullptr == _szFindString)
		return nullptr;

	// find_if() 함수를 통해 검색
	COMPONENT_MAP::iterator find_iter = find_if(m_ComponentMap.begin(), m_ComponentMap.end(), IsEqual_tchar(_szFindString));

	if(m_ComponentMap.end() == find_iter)
		return nullptr;

	return find_iter->second;
}


// Free
void CGameObject::Free(){
	SafeRelease(m_pGraphicDevice);
}
