#ifndef __UI_H__
#define __UI_H__

#include "GameObject.h"

BEGIN(Engine)

class CVIBufferRect;
class CRenderer;
class CTransform;
class CShader;
class CTexture;

class ENGINE_DLL CImageUI abstract:
	public CGameObject{
public:
	typedef struct TagUIDesc{
		_float fX = 0.f;
		_float fY = 0.f;
		_float fSizeX = 0.f;
		_float fSizeY = 0.f;
		_uint uiSceneNumber = 0;
		const _tchar* szTextureName;
	}UI_DESC;

protected:
	explicit CImageUI(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CImageUI(const CImageUI& _UI);
	virtual ~CImageUI() = default;

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
	void SetUIEnable(_bool _bIsUIEnable){
		m_bIsUIEnable = _bIsUIEnable;
	}

protected:
	HRESULT SetShaderValue(D3DXHANDLE _szVarName, void* _pValue, _uint _uiSizeValue);
	_uint GetShaderPass(){
		return m_uiShaderPass;
	}
	void SetShaderPass(_uint _uiShaderPass){
		m_uiShaderPass = _uiShaderPass;
	}

protected:
	CTransform* m_pTransformCom = nullptr;
	_float m_fX = 0.f;
	_float m_fY = 0.f;
	_float m_fSizeX = 0.f;
	_float m_fSizeY = 0.f;
	_bool m_bIsUIEnable = true;

private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CVIBufferRect* m_pVIBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	_float m_fWinCX = 0.f;
	_float m_fWinCY = 0.f;

	_uint m_uiShaderPass = 0;

protected:
	HRESULT SettingShader();

public:
	virtual void Free();
};

END

#endif // !__UI_H__
