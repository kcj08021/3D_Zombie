#include "..\Header\ImageUI.h"
#include "..\Header\VIBufferRect.h"
#include "..\Header\Renderer.h"
#include "..\Header\Transform.h"
#include "..\Header\Shader.h"
#include "..\Header\Texture.h"



CImageUI::CImageUI(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_bIsUIEnable(true)
	, m_fX(0.f), m_fY(0.f), m_fSizeX(0.f), m_fSizeY(0.f)
	, m_uiShaderPass(0)
	, m_fWinCX(0.f), m_fWinCY(0.f){
}

CImageUI::CImageUI(const CImageUI & _UI)
	: CGameObject(_UI)
	, m_pRendererCom(_UI.m_pRendererCom)
	, m_pShaderCom(_UI.m_pShaderCom)
	, m_pTextureCom(_UI.m_pTextureCom)
	, m_pTransformCom(_UI.m_pTransformCom)
	, m_pVIBufferCom(_UI.m_pVIBufferCom)
	, m_bIsUIEnable(true)
	, m_fX(_UI.m_fX), m_fY(_UI.m_fY), m_fSizeX(_UI.m_fSizeX), m_fSizeY(_UI.m_fSizeY)
	, m_uiShaderPass(_UI.m_uiShaderPass)
	, m_fWinCX(_UI.m_fWinCX), m_fWinCY(_UI.m_fWinCY){
	SafeAddRef(_UI.m_pRendererCom);
	SafeAddRef(_UI.m_pShaderCom);
	SafeAddRef(_UI.m_pVIBufferCom);
}

HRESULT CImageUI::InitGameObject_Proto(){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	m_pShaderCom = CShader::CreateProto(CGameObject::GetGraphicDevice(), L"../Bin/ShaderFile/UIShader.fx");

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pVIBufferCom), L"VIBufferCom", L"VIBufferRectCom_Proto", 0)))
		return E_FAIL;

	D3DVIEWPORT9 ViewPort;
	CGameObject::GetGraphicDevice()->GetViewport(&ViewPort);

	m_fWinCX = static_cast<_float>(ViewPort.Width);
	m_fWinCY = static_cast<_float>(ViewPort.Height);

	return NOERROR;
}

HRESULT CImageUI::InitGameObject_Clone(void * _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	UI_DESC* pDesc = reinterpret_cast<UI_DESC*>(_pArgument);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	CTransform::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.fRotationPerSec = 0.f;
	TransformDesc.fSpeedPerSec = 0.f;

	_matrix ScaleMatrix, TransMatrix, WorldMatrix;
	D3DXMatrixScaling(&ScaleMatrix, m_fSizeX, m_fSizeY, 1.f);
	D3DXMatrixTranslation(&TransMatrix, m_fWinCX*0.5f - m_fX, m_fWinCY*0.5f - m_fY, 0.f);

	TransformDesc.pMatrixWorldSetting = &(WorldMatrix = ScaleMatrix * TransMatrix);

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &TransformDesc)))
		return E_FAIL;

	if(nullptr != pDesc->szTextureName)
		if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTextureCom), L"TextureCom", pDesc->szTextureName, pDesc->uiSceneNumber)))
			return E_FAIL;	

	return NOERROR;
}

HRESULT CImageUI::LateInitGameObject(){
	return NOERROR;
}

HRESULT CImageUI::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CImageUI::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	if(m_bIsUIEnable)
		m_pRendererCom->AddRenderList(CRenderer::RENDER_PRE_UI, this);

	return NOERROR;
}

HRESULT CImageUI::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pVIBufferCom)
		return E_FAIL;

	if(FAILED(SettingShader()))
		return E_FAIL;

	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(m_uiShaderPass);

	m_pVIBufferCom->RenderVIBufferRect();

	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return NOERROR;
}

HRESULT CImageUI::SetShaderValue(D3DXHANDLE _szVarName, void * _pValue, _uint _uiSizeValue){
	return m_pShaderCom->SetValue(_szVarName, _pValue, _uiSizeValue);
}

HRESULT CImageUI::SettingShader(){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pTransformCom)
		return E_FAIL;

	_matrix matView, matProj;
	
	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, m_fWinCX, m_fWinCY, 0.f, 1.f);

	m_pShaderCom->SetValue("g_matWorld", m_pTransformCom->GetWorldMatrixPointer(), sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture());

	return NOERROR;
}

void CImageUI::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pVIBufferCom);

	CGameObject::Free();
}
