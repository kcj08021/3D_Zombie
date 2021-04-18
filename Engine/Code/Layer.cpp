#include "..\Header\Layer.h"
#include "..\Header\GameObject.h"

CLayer::CLayer(){
}

HRESULT CLayer::InitLayer(){
	return NOERROR;
}

HRESULT CLayer::UpdateLayer(_double _dDeltaTime){
	for(OBJ_LIST::iterator iter = m_ObjectList.begin(); iter != m_ObjectList.end();){
		if(nullptr != *iter){
			(*iter)->UpdateGameObject(_dDeltaTime);

			if((*iter)->GetIsDeadObject()){
				SafeRelease(*iter);
				iter = m_ObjectList.erase(iter);
			} else
				++iter;
		} else
			iter = m_ObjectList.erase(iter);
	}

	return NOERROR;
}

HRESULT CLayer::LateUpdateLayer(_double _dDeltaTime){
	for(auto& elem : m_ObjectList)
		if(nullptr != elem)
			elem->LateUpdateGameObject(_dDeltaTime);

	return NOERROR;
}

HRESULT CLayer::AddGameObject(CGameObject * _pObj){
	if(nullptr == _pObj)
		return E_FAIL;

	m_ObjectList.push_back(_pObj);

	return NOERROR;
}

HRESULT CLayer::ReleaseCloneObject(CGameObject * _pObj){
	if(nullptr == _pObj)
		return E_FAIL;
	OBJ_LIST::iterator find_iter = find_if(m_ObjectList.begin(), m_ObjectList.end(), IsEqual_Address(_pObj));
	
	if(m_ObjectList.end() == find_iter)
		return E_FAIL;

	SafeRelease(*find_iter);
	m_ObjectList.erase(find_iter);

	return NOERROR;
}

void CLayer::Free(){
	for(auto& elem : m_ObjectList){
		SafeRelease(elem);
	}
	m_ObjectList.clear();
}

CLayer * CLayer::Create(){
	CLayer* pInstance = new CLayer;
	if(FAILED(pInstance->InitLayer())){
		MSG_BOX(L"CLayer Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}
