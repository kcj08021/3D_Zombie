#include "..\Header\Target.h"
#include "..\Header\VIBufferViewportRect.h"
#include "..\Header\Shader.h"


CTarget::CTarget(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: m_pGraphicDevice(_pGraphicDevice)
	, m_pTexture(nullptr)
	, m_pSurface(nullptr)
	, m_pOldSurface(nullptr)
	, m_pVIBufferCom(nullptr){
	SafeAddRef(m_pGraphicDevice);
}

HRESULT CTarget::InitTarget(_uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor){
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;

	if(FAILED(D3DXCreateTexture(m_pGraphicDevice, _uiWidth, _uiHeight, 1, D3DUSAGE_RENDERTARGET, _Format, D3DPOOL_DEFAULT, &m_pTexture)))
		return E_FAIL;
	
	if(FAILED(m_pTexture->GetSurfaceLevel(0, &m_pSurface)))
		return E_FAIL;

	m_ClearColor = _ClearColor;

	return NOERROR;
}

HRESULT CTarget::ClearRenderTarget(){
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;
	
	BeginRenderTargat(0);

	m_pGraphicDevice->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	EndRenderTargat(0);

	return NOERROR;
}

HRESULT CTarget::BeginRenderTargat(_uint _uiIndex){
	if(nullptr == m_pGraphicDevice
		|| nullptr == m_pSurface)
		return E_FAIL;

	m_pGraphicDevice->GetRenderTarget(_uiIndex, &m_pOldSurface);

	if(FAILED(m_pGraphicDevice->SetRenderTarget(_uiIndex, m_pSurface)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::EndRenderTargat(_uint _uiIndex){
	if(nullptr == m_pGraphicDevice
		|| nullptr == m_pSurface)
		return E_FAIL;

	if(FAILED(m_pGraphicDevice->SetRenderTarget(_uiIndex, m_pOldSurface)))
		return E_FAIL;

	SafeRelease(m_pOldSurface);

	return NOERROR;
}

HRESULT CTarget::SettingDebugBuffer(_float _fStartX, _float _fStartY, _float _fSizeX, _float _fSizeY){
	m_pVIBufferCom = CVIBufferViewportRect::CreateProto(m_pGraphicDevice, _fStartX, _fStartY, _fSizeX, _fSizeY);

	if(nullptr == m_pVIBufferCom)
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::RenderDebugBuffer(){
	if(nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pGraphicDevice->SetTexture(0, m_pTexture);
	m_pVIBufferCom->RenderVIBuffer();

	return NOERROR;
}

HRESULT CTarget::SettingOnShader(CShader * _pShader, const char * _szConstantTable){
	if(nullptr == _pShader 
		|| nullptr == _szConstantTable
		|| nullptr == m_pTexture)
		return E_FAIL;
	
	return _pShader->SetTexture(_szConstantTable, m_pTexture);
}

CTarget * CTarget::CreateTarget(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiWidth, _uint _uiHeight, D3DFORMAT _Format, D3DXCOLOR _ClearColor){
	CTarget* pInstance = new CTarget(_pGraphicDevice);
	if(FAILED(pInstance->InitTarget(_uiWidth, _uiHeight, _Format, _ClearColor))){
		SafeRelease(pInstance);
		MSG_BOX(L"CTarget Create Failure");
	}
	
	return pInstance;
}

void CTarget::Free(){
	SafeRelease(m_pOldSurface);
	SafeRelease(m_pSurface);
	SafeRelease(m_pTexture);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pGraphicDevice);
}
