#include "..\Header\Light.h"
#include "..\Header\Shader.h"
#include "..\Header\VIBufferViewportRect.h"
#include "..\Header\Pipeline.h"

// 생성자
CLight::CLight(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: m_pGraphicDevice(_pGraphicDevice)
	, m_fViewZ(0.f)
	, m_bIsTurnOn(false){
	SafeAddRef(m_pGraphicDevice);
	::ZeroMemory(&m_tLightDesc, sizeof(D3DLIGHT9));
}

// Initialize
HRESULT CLight::InitLight(const D3DLIGHT9 * _pLightDesc){
	if(nullptr == _pLightDesc)
		return E_FAIL;

	m_tLightDesc = *_pLightDesc;

	D3DVIEWPORT9 viewPort;
	m_pGraphicDevice->GetViewport(&viewPort);

	m_pRenderBuffer = CVIBufferViewportRect::CreateProto(m_pGraphicDevice, 0.f - 0.5f, 0.f - 0.5f, static_cast<_float>(viewPort.Width), static_cast<_float>(viewPort.Height));
	if(nullptr == m_pRenderBuffer)
		return E_FAIL;

	m_bIsTurnOn = true;

	return NOERROR;
}

// 라이트 렌더(디퍼드)
HRESULT CLight::RenderLight(CShader * _pShader){
	if(nullptr == m_pRenderBuffer)
		return E_FAIL;

	if(!m_bIsTurnOn)
		return NOERROR;

	_uint uiPassIndex = 0;

	_pShader->SetValue("g_vLightDiffuse", &m_tLightDesc.Diffuse, sizeof(_vec4));
	_pShader->SetValue("g_vLightSpecular", &m_tLightDesc.Specular, sizeof(_vec4));
	_pShader->SetValue("g_vLightAmbient", &m_tLightDesc.Ambient, sizeof(_vec4));

	if(D3DLIGHT_DIRECTIONAL == m_tLightDesc.Type){
		uiPassIndex = 0;

		_pShader->SetValue("g_vLightDir", &_vec4(m_tLightDesc.Direction, 0.f), sizeof(_vec4));
	} else if(D3DLIGHT_POINT == m_tLightDesc.Type){
		if(ComputeViewZ())
			return E_FAIL;

		uiPassIndex = 1;

		_pShader->SetValue("g_vLightPosition", &_vec4(m_tLightDesc.Position, 1.f), sizeof(_vec4));
		_pShader->SetValue("g_fLightRange", &m_tLightDesc.Range, sizeof(_float));
	}

	_pShader->CommitChange();

	_pShader->BeginPass(uiPassIndex);

	m_pRenderBuffer->RenderVIBuffer();

	_pShader->EndPass();

	return NOERROR;
}

// 라이트의 뷰 스페이스상 Z값 계산
HRESULT CLight::ComputeViewZ(){
	CPipeline* pPipeline = CPipeline::GetInstance();
	if(nullptr == pPipeline)
		return E_FAIL;

	SafeAddRef(pPipeline);

	_matrix matCameraWorld = pPipeline->GetCameraWorldMatrix();
	_vec3 vLightPos(m_tLightDesc.Position);

	m_fViewZ = D3DXVec3Length(&(vLightPos - *reinterpret_cast<_vec3*>(matCameraWorld.m[3])));

	SafeRelease(pPipeline);

	return NOERROR;
}
// 생성 함수(static)
CLight * CLight::Create(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9 * _pLightDesc){
	CLight* pInstance = new CLight(_pGraphicDevice);
	if(FAILED(pInstance->InitLight(_pLightDesc))){
		SafeRelease(pInstance);
		MSG_BOX(L"CLight Create Failure");
	}
	
	return pInstance;
}

// Free
void CLight::Free(){
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pRenderBuffer);
}
