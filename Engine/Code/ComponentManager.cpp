#include "..\Header\ComponentManager.h"

// �̱��� ����
IMPLEMENT_SINGLETON(CComponentManager)

// ������
CComponentManager::CComponentManager()
	:m_pComponentMapArray(nullptr)
	, m_uiSceneCount(0){
}

// �� ������ ���缭 �����̸� �迭 ���� �Ҵ�
HRESULT CComponentManager::ReserveComponentMapArray(_uint _uiSceneCount){
	if(0 == _uiSceneCount)
		return E_FAIL;

	m_pComponentMapArray = new COMPONENT_MAP[_uiSceneCount];
	m_uiSceneCount = _uiSceneCount;

	return NOERROR;
}

// ������Ÿ�� ������Ʈ �߰�
HRESULT CComponentManager::AddProtoComponent(const _tchar * _szKey, _uint _uiIndex, CComponent * _pComponent){
	if(nullptr == _szKey || _uiIndex >= m_uiSceneCount || nullptr == _pComponent)
		return E_FAIL;

	if(nullptr != FindPrototype(_szKey, _uiIndex))
		return E_FAIL;

	m_pComponentMapArray[_uiIndex][_szKey] = _pComponent;

	return NOERROR;
}

// ������Ÿ���� �����Ͽ� ��ȯ
CComponent* CComponentManager::AddCloneComponent(const _tchar * _szProtoKey, _uint _uiIndex, void* _pArgument){
	CComponent* pComponent = FindPrototype(_szProtoKey, _uiIndex);

	if(nullptr == pComponent)
		return nullptr;
	
	return  pComponent->CreateClone(_pArgument);;
}

// ��ϵǾ� �ִ� ������Ÿ�� ������Ʈ�� ����
HRESULT CComponentManager::ReleaseProtoComponent(const _tchar * _szKey, _uint _uiIndex){
	if(nullptr == _szKey
		|| m_uiSceneCount <= _uiIndex)
		return E_FAIL;

	CComponent* pComponent = FindPrototype(_szKey, _uiIndex);
	if(nullptr == pComponent)
		return E_FAIL;

	SafeRelease(pComponent);
	m_pComponentMapArray[_uiIndex].erase(_szKey);

	return NOERROR;
}


// �����̳� �˻� �Լ�
CComponent * CComponentManager::FindPrototype(const _tchar * _szFindString, _uint _uiIndex){
	if(nullptr == _szFindString || _uiIndex >= m_uiSceneCount)
		return nullptr;
	
	// find_if() �Լ��� ���� �˻�
	COMPONENT_MAP::iterator find_iter = find_if(m_pComponentMapArray[_uiIndex].begin(), m_pComponentMapArray[_uiIndex].end(), IsEqual_tchar(_szFindString));

	if(m_pComponentMapArray[_uiIndex].end() == find_iter)
		return nullptr;

	return find_iter->second;
}

// Free
void CComponentManager::Free(){
	for(size_t i = 0; i < m_uiSceneCount; ++i){
		for(auto& elem : m_pComponentMapArray[i])
			SafeRelease(elem.second);
		m_pComponentMapArray[i].clear();
	}
	SafeDelete_Array(m_pComponentMapArray);
}
