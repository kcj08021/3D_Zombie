#ifndef __EFFECT_H__
#define __EFFECT_H__
#include "GameObject.h"

BEGIN(Engine)
class CVIBufferRect;
class CRenderer;
class CTransform;
class CShader;
class CTexture;

class ENGINE_DLL CEffect abstract:
	public CGameObject{
public:
	enum EFFECT_TYPE{SIMPLE, ANIMATION, RANDOM};
public:
	typedef struct tagEffectDesc{
		_float fEffectPlayTime = 0.f;
		_uint uiEffectPoolCount = 1;
		_matrix* pWorldMatrix = nullptr;
		_uint uiSceneNumber = 0;
		const _tchar* szShaderFileName = nullptr;
		_uint szShaderPassNum = 0;
		EFFECT_TYPE eType;
		const _tchar* szTextureName = nullptr;
		_uint uiTextureCount = 0;
		_float fMaxFrame = 0.f;
		_uint flagBillboardXYZ;
	}EFFECT_DESC;
	typedef struct tagEffectPoolStruct{
		_vec3 vPos = _vec3(0.f, 0.f, 0.f);
		_bool bIsEffectOn = false;
		_float fFrame = 0.f;
		_bool bIsAnimationEnd = false;
		_float fViewZ;
	}EFFECT_STRUCT;
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CEffect(const CEffect& _Effect);
	virtual ~CEffect() = default;

protected:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);


public:
	HRESULT SetEffectPosition(_vec3 _vPos);
	void SetEffectStatus(_bool _IsEffectOn, _uint _uiIndex = 0);


protected:
	CTransform* m_pTransformCom = nullptr;

private:
	CRenderer* m_pRendererCom = nullptr;
	CVIBufferRect* m_pVIBufferCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	_uint m_uiShaderPassNum = 0;


	EFFECT_TYPE m_eType;
	CTexture* m_pTextureCom = nullptr;
	_uint m_uiTextureCount = 0;

	_float m_fPlayTime = 0.f;
	_float m_fMaxFrame = 0.f;

	vector<EFFECT_STRUCT> m_EffectPool;
	_uint m_uiEffectPoolSize = 0;
	_uint m_uiCurrIndex = 0;

	_bool m_bIsEffectOn = true;

private:
	HRESULT SettingBillboard();
	HRESULT SettingShader(const EFFECT_STRUCT*);

public:
	virtual void Free();
};

END

#endif // !__EFFECT_H__
