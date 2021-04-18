#pragma once
#include "Base.h"

BEGIN(Engine)
class CEffect;

class CEffectManager final : public CBase{
public:
	DECLARE_SINGLETON(CEffectManager)

private:
	// 생성자
	explicit CEffectManager();
	// 소멸자
	virtual ~CEffectManager() = default;

public:
	// 사용이 끝난 씬 할당 해제
	HRESULT SceneClear(_uint _uiScene);

	HRESULT ReserveEffectMap(_uint _uiSceanCount);
	HRESULT AddEffect(const _tchar* _szEffectKey, const _tchar* _szObjectPrototypeKey, _uint _uiSceanNum, const _tchar* _szObjectLayerKey, void* _pArgument);
	CEffect* GetEffect(const _tchar* _szEffectKey) const;


private:
	_uint m_uiSceanCount = 0;
	unordered_map<const _tchar*, CEffect*>* m_pEffectMap = nullptr;
	typedef unordered_map<const _tchar*, CEffect*> EFFECT_MAP;

public:
	// Free
	virtual void Free();
};

END