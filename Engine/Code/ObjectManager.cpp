#include "..\Header\ObjectManager.h"
#include "..\Header\Layer.h"

// �̱��� ����
IMPLEMENT_SINGLETON(CObjectManager)

// ������
CObjectManager::CObjectManager(){
}

list<CGameObject*>* CObjectManager::GetObjectList(_uint _uiIndex, const _tchar* _szLayer){
	CLayer* pLayer = FindLayer(_uiIndex, _szLayer);
	if(nullptr == pLayer)
		return nullptr;
	
	return pLayer->GetList();
}

// �� ������ ���缭 �����̸� �迭 ���� �Ҵ�
HRESULT CObjectManager::ReserveLayerMapArray(_uint _uiSceneCount){
	if(0 == _uiSceneCount)
		return E_FAIL;

	m_pLayerMapArray = new LAYER_MAP[_uiSceneCount];
	m_uiSceneCount = _uiSceneCount;

	return NOERROR;
}

// Update
HRESULT CObjectManager::UpdateObjectMgr(_double _dDeltaTime){
	for(size_t i = 0; i <m_uiSceneCount; i++){
		for(auto& elem : m_pLayerMapArray[i])
			if(nullptr != elem.second)
				elem.second->UpdateLayer(_dDeltaTime);
	}

	return NOERROR;
}

// LateUpdate
HRESULT CObjectManager::LateUpdateObjectMgr(_double _dDeltaTime){
	for(size_t i = 0; i <m_uiSceneCount; i++){
		for(auto& elem : m_pLayerMapArray[i])
			if(nullptr != elem.second)
				elem.second->LateUpdateLayer(_dDeltaTime);
	}

	return NOERROR;
}

// ����� ���� �� �Ҵ� ����
HRESULT CObjectManager::SceneClear(_uint _uiScene){
	if(_uiScene >= m_uiSceneCount)
		return E_FAIL;

	for(auto& elem : m_pLayerMapArray[_uiScene])
		SafeRelease(elem.second);

	m_pLayerMapArray[_uiScene].clear();

	return E_NOTIMPL;
}

// ������Ÿ�� ������Ʈ �߰�
HRESULT CObjectManager::AddProtoObject(const _tchar * _szKey, CGameObject * _pObj){
	if(nullptr == _szKey || nullptr == _pObj)
		return E_FAIL;

	if(nullptr != FindPrototype(_szKey))
		return E_FAIL;

	m_PrototypeMap[_szKey] = _pObj;

	return NOERROR;
}

// ������Ÿ���� �����Ͽ� ���̾ �߰�
HRESULT CObjectManager::AddCloneObjectToLayer(const _tchar * _szProtoKey, _uint _uiIndex, const _tchar * _szLayerKey, void * _pArgument, CGameObject** _ppOutClone){
	if(m_uiSceneCount <= _uiIndex)
		return E_FAIL;

	CGameObject* pPrototype = FindPrototype(_szProtoKey);
	if(nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pObj = pPrototype->CreateClone(_pArgument);
	if(nullptr == pObj)
		return E_FAIL;

	CLayer* pLayer = FindLayer(_uiIndex, _szLayerKey);

	if(nullptr == pLayer){
		pLayer = CLayer::Create();
		if(nullptr == pLayer)
			return E_FAIL;

		pLayer->AddGameObject(pObj);
		m_pLayerMapArray[_uiIndex][_szLayerKey] = pLayer;
	} else{
		pLayer->AddGameObject(pObj);
	}

	if(nullptr != _ppOutClone)
		*_ppOutClone = pObj;
	
	return NOERROR;
}

HRESULT CObjectManager::ReleaseLayer(_uint _uiIndex, const _tchar * _szLayerKey){
	// find_if() �Լ��� ���� �˻�
	LAYER_MAP::iterator find_iter = find_if(m_pLayerMapArray[_uiIndex].begin(), m_pLayerMapArray[_uiIndex].end(), IsEqual_tchar(_szLayerKey));
	
	if(m_pLayerMapArray[_uiIndex].end() == find_iter)
		return E_FAIL;

	SafeRelease(find_iter->second);

	m_pLayerMapArray[_uiIndex].erase(find_iter);

	return NOERROR;
}

HRESULT CObjectManager::ReleaseCloneObject(_uint _uiIndex, const _tchar * _szLayerKey, CGameObject * _pSelectedObj){
	// find_if() �Լ��� ���� �˻�
	LAYER_MAP::iterator find_iter = find_if(m_pLayerMapArray[_uiIndex].begin(), m_pLayerMapArray[_uiIndex].end(), IsEqual_tchar(_szLayerKey));
	if(m_pLayerMapArray[_uiIndex].end() == find_iter)
		return E_FAIL;

	return find_iter->second->ReleaseCloneObject(_pSelectedObj);
}

// ������Ÿ�� �����̳� ���� �˻�
CGameObject* CObjectManager::FindPrototype(const _tchar* _szFindString){
	// find_if() �Լ��� ���� �˻�
	PROTO_MAP::iterator find_iter = find_if(m_PrototypeMap.begin(), m_PrototypeMap.end(), IsEqual_tchar(_szFindString));

	if(m_PrototypeMap.end() == find_iter)
		return nullptr;
	
	return find_iter->second;
}

// ���̾� �����̳� ���� �˻�
CLayer * CObjectManager::FindLayer(_uint _uiIndex, const _tchar * _szFindString){
	// find_if() �Լ��� ���� �˻�
	LAYER_MAP::iterator find_iter = find_if(m_pLayerMapArray[_uiIndex].begin(), m_pLayerMapArray[_uiIndex].end(), IsEqual_tchar(_szFindString));

	if(m_pLayerMapArray[_uiIndex].end() == find_iter)
		return nullptr;

	return find_iter->second;
}

// Free
void CObjectManager::Free(){
	for(_uint i = 0; i < m_uiSceneCount; ++i){
		for(auto& elem: m_pLayerMapArray[i])
			SafeRelease(elem.second);
		m_pLayerMapArray[i].clear();
	}
	SafeDelete_Array(m_pLayerMapArray);

	for(auto& elem : m_PrototypeMap){
		SafeRelease(elem.second);
	}

	m_PrototypeMap.clear();
}
