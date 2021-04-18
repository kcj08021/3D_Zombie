#include "..\Header\TextUI.h"

USING(Engine)

CTextUI::CTextUI(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pFont(nullptr)
	, m_pRendererCom(nullptr)
	, m_pString(nullptr)
	, m_bIsUIEnable(true){
	::ZeroMemory(&m_tRect, sizeof(RECT));
}

CTextUI::CTextUI(const CTextUI & _TextUI)
	: CGameObject(_TextUI)
	, m_pFont(_TextUI.m_pFont)
	, m_pRendererCom(_TextUI.m_pRendererCom)
	, m_pString(_TextUI.m_pString)
	, m_tRect(_TextUI.m_tRect)
	, m_bIsUIEnable(true){
	SafeAddRef(m_pFont);
	SafeAddRef(m_pRendererCom);
}

HRESULT CTextUI::InitGameObject_Proto(D3DXFONT_DESC* _pDesc){
	if(FAILED(D3DXCreateFontIndirect(CGameObject::GetGraphicDevice(), _pDesc, &m_pFont))){
		return E_FAIL;
	}

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTextUI::InitGameObject_Clone(void * _pArgument){
	TextUI_DESC tDesc = *reinterpret_cast<TextUI_DESC*>(_pArgument);
	if(0 == tDesc.uiTextLength)
		return E_FAIL;

	m_tRect.top = static_cast<LONG>(tDesc.fY - (tDesc.fSizeY* 0.5f));
	m_tRect.left = static_cast<LONG>(tDesc.fX - (tDesc.fSizeX* 0.5f));
	m_tRect.bottom = static_cast<LONG>(tDesc.fY + (tDesc.fSizeY* 0.5f));
	m_tRect.right = static_cast<LONG>(tDesc.fX + (tDesc.fSizeX* 0.5f));

	m_pString = new TCHAR[tDesc.uiTextLength];

	return NOERROR;
}

HRESULT CTextUI::LateInitGameObject(){
	return NOERROR;
}

HRESULT CTextUI::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CTextUI::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	if(m_bIsUIEnable)
		m_pRendererCom->AddRenderList(CRenderer::RENDER_POST_UI, this);

	return NOERROR;
}

HRESULT CTextUI::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom
		&& nullptr == m_pFont
		&& nullptr == m_pString)
		return E_FAIL;

	m_pFont->DrawText(nullptr, m_pString, -1, &m_tRect, DT_CENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	return NOERROR;
}

void CTextUI::Free(){
	SafeRelease(m_pFont);
	SafeRelease(m_pRendererCom);
	SafeDelete_Array(m_pString);

	CGameObject::Free();
}
