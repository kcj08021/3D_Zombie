#include "..\Header\TargetManager.h"
#include "..\Header\Shader.h"

IMPLEMENT_SINGLETON(CTargetManager)

CTargetManager::CTargetManager(){
}

HRESULT CTargetManager::AddRenderTarget(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar *_szTargetKey, _uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor){
	if(nullptr != FindeRenderTarget(_szTargetKey))
		return E_FAIL;

	CTarget* pTarget = CTarget::CreateTarget(_pGraphicDevice, _uiWidth, _uiHeight, _Format, _ClearColor);
	if(nullptr == pTarget)
		return E_FAIL;

	m_RenderTargetMap[_szTargetKey] = pTarget;
	
	return NOERROR;
}

HRESULT CTargetManager::SettingMultiRenderTarget(const _tchar * _szMRTKey, const _tchar * _szTargetKey){
	CTarget* find_Target = FindeRenderTarget(_szTargetKey);
	if(nullptr == find_Target)
		return E_FAIL;

	auto pList = FindMultiRenderTargat(_szMRTKey);
	if(nullptr == pList){
		list<CTarget*> MRTList;
		MRTList.push_back(find_Target);

		m_MultiRenderTargetMap[_szMRTKey] = MRTList;
	} else{
		auto find_iter = find_if(pList->begin(), pList->end(), IsEqual_Address(find_Target));
		if(pList->end() != find_iter)
			return E_FAIL;

		pList->push_back(find_Target);
	}

	SafeAddRef(find_Target);

	return NOERROR;
}

HRESULT CTargetManager::SettingDebugBuffer(const _tchar * _szKey, _float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY){
	CTarget* pTarget = FindeRenderTarget(_szKey);
	if(nullptr == pTarget)
		return E_FAIL;

	return pTarget->SettingDebugBuffer(_fStartX, _fStartY, _fSizeX, _fSizeY);
}

HRESULT CTargetManager::SettingOnShader(CShader * _pShader, const char * _szConstantTable, const _tchar * _szTargetKey){
	CTarget* pTarget = FindeRenderTarget(_szTargetKey);
	if(nullptr == pTarget)
		return E_FAIL;

	return pTarget->SettingOnShader(_pShader, _szConstantTable);
}



HRESULT CTargetManager::BeginMultiRenderTarget(const _tchar * _szMRTKey){
	auto pList = FindMultiRenderTargat(_szMRTKey);
	if(nullptr == pList)
		return E_FAIL;

	for(auto& elem : *pList){
		if(FAILED(elem->ClearRenderTarget()))
			return E_FAIL;
	}

	_uint uiIndex = 0;
	for(auto& elem : *pList){
		if(FAILED(elem->BeginRenderTargat(uiIndex++)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTargetManager::EndMultiRenderTarget(const _tchar * _szMRTKey){
	auto pList = FindMultiRenderTargat(_szMRTKey);
	if(nullptr == pList)
		return E_FAIL;

	_uint uiIndex = 0;
	for(auto& elem : *pList){
		if(FAILED(elem->EndRenderTargat(uiIndex++)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTargetManager::RenderDebugBuffer(const _tchar * _szMRTKey){
	auto pList = FindMultiRenderTargat(_szMRTKey);
	if(nullptr == pList)
		return E_FAIL;

	for(auto& elem : *pList){
		if(FAILED(elem->RenderDebugBuffer()))
			return E_FAIL;
	}

	return NOERROR;
}

CTarget * CTargetManager::FindeRenderTarget(const _tchar * _szKey){
	auto find_iter = find_if(m_RenderTargetMap.begin(), m_RenderTargetMap.end(), IsEqual_tchar(_szKey));
	
	if(m_RenderTargetMap.end() == find_iter)
		return nullptr;

	return find_iter->second;
}

list<CTarget*>* CTargetManager::FindMultiRenderTargat(const _tchar * _szKey){
	auto find_iter = find_if(m_MultiRenderTargetMap.begin(), m_MultiRenderTargetMap.end(), IsEqual_tchar(_szKey));

	if(m_MultiRenderTargetMap.end() == find_iter)
		return nullptr;

	return &find_iter->second;
}

void CTargetManager::Free(){
	for(auto& elem : m_RenderTargetMap)
		SafeRelease(elem.second);
	m_RenderTargetMap.clear();

	for(auto& elemPair : m_MultiRenderTargetMap){
		for(auto& elem : elemPair.second)
			SafeRelease(elem);
		elemPair.second.clear();
	}
	m_MultiRenderTargetMap.clear();
}
