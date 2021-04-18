#include "..\Header\LightManager.h"
#include "..\Header\Light.h"

// 싱글톤 구현
IMPLEMENT_SINGLETON(CLightManager)

// 생성자
CLightManager::CLightManager(){
}

const D3DLIGHT9* CLightManager::GetLight(_uint _uiIndex) const{
	if(m_LightList.size() <= _uiIndex)
		return nullptr;

	LIGHT_LIST::const_iterator iter = m_LightList.begin();
	_uint uiCount = 0;

	for(; iter!= m_LightList.end(); ++iter){
		if((*iter)->IsTurnOn())
			if(_uiIndex == uiCount){
				break;
			} else{
				++uiCount;
			}
	}

	if(iter == m_LightList.end())
		return nullptr;
	else
		return (*iter)->GetLight();
}

HRESULT CLightManager::RenderLight(CShader* _pShader){
	for(auto& elem : m_LightList)
		if(nullptr != elem)
			elem->RenderLight(_pShader);

	m_LightList.sort([](CLight* _pSrc, CLight* _pDest) -> _bool{
		return _pSrc->GetViewZ() < _pDest->GetViewZ();
	});
	
	return NOERROR;
}

// 라이트 추가
HRESULT CLightManager::AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc, CLight** _ppOutLight){
	CLight* pLight = CLight::Create(_pGraphicDevice, _pLightDesc);
	if(nullptr == pLight)
		return E_FAIL;

	m_LightList.push_back(pLight);

	if(nullptr != _ppOutLight)
		*_ppOutLight = pLight;

	return NOERROR;
}

// 라이트 삭제
HRESULT CLightManager::ReleaseLight(CLight * _pLight){
	if(nullptr == _pLight)
		return E_FAIL;

	LIGHT_LIST::iterator find_iter =  find_if(m_LightList.begin(), m_LightList.end(), IsEqual_Address(_pLight));
	if(m_LightList.end() == find_iter)
		return E_FAIL;

	SafeRelease(*find_iter);
	m_LightList.erase(find_iter);

	return NOERROR;
}

HRESULT CLightManager::ResetLight(){
	Free();

	return NOERROR;
}

// Free
void CLightManager::Free(){
	for(auto& elem : m_LightList)
		SafeRelease(elem);
	m_LightList.clear();
}
