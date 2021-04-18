#ifndef __TEXT_UI_H__
#define __TEXT_UI_H__


#include "GameObject.h"

BEGIN(Engine)

class CRenderer;

class ENGINE_DLL CTextUI abstract
	: public CGameObject{
public:
		typedef struct tagTextUIDESC{
		float fX = 0.f;
		float fY = 0.f;
		float fSizeX = 0.f;
		float fSizeY = 0.f;
		size_t uiTextLength = 0;
}TextUI_DESC;

protected:
	explicit CTextUI(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CTextUI(const CTextUI& _TextUI);
	virtual ~CTextUI() = default;

protected:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(D3DXFONT_DESC* _pDesc);
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

	// Set
public:
	void SetIsUIEnable(bool _bIsEnable){
		m_bIsUIEnable = _bIsEnable;
	}

	// Get
public:
	bool IsUIEnable(){
		return m_bIsUIEnable;
	}
	RECT* GetRect(){
		return &m_tRect;
	}

protected:
	TCHAR* m_pString = nullptr;
	bool m_bIsUIEnable = true;

private:
	LPD3DXFONT m_pFont = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	RECT m_tRect;

public:
	virtual void Free() override;
};

END

#endif // !__TEXT_UI_H__
