#include "..\Header\Renderer.h"
#include "..\Header\GameObject.h"
#include "..\Header\TargetManager.h"
#include "..\Header\LightManager.h"
#include "..\Header\Pipeline.h"
#include "..\Header\Shader.h"
#include "..\Header\VIBufferViewportRect.h"

// 생성자
CRenderer::CRenderer(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pTargetManager(CTargetManager::GetInstance())
	, m_pLightManager(CLightManager::GetInstance())
	, m_pPipeline(CPipeline::GetInstance())
	, m_pBlendShader(nullptr)
	, m_bIsRenderDebugBuffer(false){
	SafeAddRef(m_pTargetManager);
	SafeAddRef(m_pLightManager);
	SafeAddRef(m_pPipeline);
}

// Initialize with Prototype
HRESULT CRenderer::InitComponent_Proto(){
	// 렌더타겟 매니저 확인
	if(nullptr == m_pTargetManager)
		return E_FAIL;
	
	m_pBlendShader = CShader::CreateProto(CComponent::GetGraphicDevice(), L"../Bin/ShaderFile/BlendShader.fx");
	if(nullptr == m_pBlendShader)
		return E_FAIL;

	m_pLightAccShader = CShader::CreateProto(CComponent::GetGraphicDevice(), L"../Bin/ShaderFile/LightAccShader.fx");
	if(nullptr == m_pLightAccShader)
		return E_FAIL;


	// 현재 장치가 그리는 뷰포트의 정보 얻어옴
	D3DVIEWPORT9 viewPort;
	if(FAILED(CComponent::GetGraphicDevice()->GetViewport(&viewPort)))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(디퓨즈)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"DiffuseTarget", viewPort.Width, viewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"DiffuseTarget", 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"DefferedMRT", L"DiffuseTarget")))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(노말)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"NormalTarget", viewPort.Width, viewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"NormalTarget", 200.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"DefferedMRT", L"NormalTarget")))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(스펙큘러 색상)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"SpecularColorTarget", viewPort.Width, viewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"SpecularColorTarget", 400.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"DefferedMRT", L"SpecularColorTarget")))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(깊이)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"DepthTarget", viewPort.Width, viewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"DepthTarget", 600.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"DefferedMRT", L"DepthTarget")))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(명암)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"ShadeTarget", viewPort.Width, viewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"ShadeTarget", 0.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"LightAccMRT", L"ShadeTarget")))
		return E_FAIL;

	// 뷰포트의 크기로 렌더타겟 생성(스펙큘러)
	if(FAILED(m_pTargetManager->AddRenderTarget(CComponent::GetGraphicDevice(), L"SpecularTarget", viewPort.Width, viewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingDebugBuffer(L"SpecularTarget", 200.f, 200.f, 200.f, 200.f)))
		return E_FAIL;
	if(FAILED(m_pTargetManager->SettingMultiRenderTarget(L"LightAccMRT", L"SpecularTarget")))
		return E_FAIL;

	m_pRenderBuffer = CVIBufferViewportRect::CreateProto(CComponent::GetGraphicDevice(), 0.f - 0.5f, 0.f - 0.5f, static_cast<_float>(viewPort.Width), static_cast<_float>(viewPort.Height));
	if(nullptr == m_pRenderBuffer)
		return E_FAIL;

	return NOERROR;	
}

// Initialize with Clone
HRESULT CRenderer::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

// 렌더할 오브젝트 추가
HRESULT CRenderer::AddRenderList(RENDER_GROUP _eGroup, CGameObject * _pObj){
	if(_eGroup >= RENDER_GROUP::RENDER_END || nullptr == _pObj)
		return E_FAIL;

	m_RenderList[_eGroup].push_back(_pObj);
	SafeAddRef(_pObj);

	return NOERROR;
}

// 렌더 시작
HRESULT CRenderer::DoRender(_double _dDeltaTime){
	if(FAILED(RenderPreGroup(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pTargetManager->BeginMultiRenderTarget(L"DefferedMRT")))
		return E_FAIL;

	if(FAILED(RenderNonAlphaGroup(_dDeltaTime)))
		return E_FAIL;
	if(FAILED(RenderPriorityGroup(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pTargetManager->EndMultiRenderTarget(L"DefferedMRT")))
		return E_FAIL;

	if(FAILED(RenderLight()))
		return E_FAIL;

	if(FAILED(BlendRenderTarget()))
		return E_FAIL;

	if(FAILED(RenderAlphaGroup(_dDeltaTime)))
		return E_FAIL;
	if(FAILED(RenderUIGroup(_dDeltaTime)))
		return E_FAIL;
	
	if(m_bIsRenderDebugBuffer){
		m_pTargetManager->RenderDebugBuffer(L"DefferedMRT");
		m_pTargetManager->RenderDebugBuffer(L"LightAccMRT");
	}

	return NOERROR;
}

HRESULT CRenderer::RenderPreGroup(_double _dDeltaTime){
	for(auto& elem : m_RenderList[RENDER_PRE]){
		elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}
	m_RenderList[RENDER_PRE].clear();

	return NOERROR;
}

HRESULT CRenderer::RenderNonAlphaGroup(_double _dDeltaTime){
	for(auto& elem : m_RenderList[RENDER_NONALPHA]){
		elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}

	m_RenderList[RENDER_NONALPHA].clear();

	return NOERROR;
}

HRESULT CRenderer::RenderPriorityGroup(_double _dDeltaTime){
	for(auto& elem : m_RenderList[RENDER_PRIORITY]){
		elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}
	m_RenderList[RENDER_PRIORITY].clear();

	return NOERROR;
}

HRESULT CRenderer::RenderLight(){
	if(nullptr == m_pTargetManager
		|| nullptr == m_pLightManager
		|| nullptr == m_pPipeline
		|| nullptr == m_pLightAccShader)
		return E_FAIL;

	if(FAILED(m_pTargetManager->BeginMultiRenderTarget(L"LightAccMRT")))
		return E_FAIL;

	_vec4 vCamerePos(m_pPipeline->GetCameraWorldMatrix().m[3]);

	_matrix matProjInv, matViewInv;

	m_pPipeline->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	m_pPipeline->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pLightAccShader->SetValue("g_vCameraPosition", &vCamerePos, sizeof(_vec4));
	m_pLightAccShader->SetValue("g_matProjInv", &matProjInv, sizeof(_matrix));
	m_pLightAccShader->SetValue("g_matViewInv", &matViewInv, sizeof(_matrix));

	m_pTargetManager->SettingOnShader(m_pLightAccShader, "g_NormalTexture", L"NormalTarget");
	m_pTargetManager->SettingOnShader(m_pLightAccShader, "g_SpecularTexture", L"SpecularColorTarget");
	m_pTargetManager->SettingOnShader(m_pLightAccShader, "g_DepthTexture", L"DepthTarget");

	m_pLightAccShader->BeginShader();

	m_pLightManager->RenderLight(m_pLightAccShader);

	m_pLightAccShader->EndShader();

	if(FAILED(m_pTargetManager->EndMultiRenderTarget(L"LightAccMRT")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::RenderAlphaGroup(_double _dDeltaTime){
	m_RenderList[RENDER_ALPHA].sort([](CGameObject* _pSrc, CGameObject* _pDest) -> _bool{
		return _pSrc->GetViewZ() > _pDest->GetViewZ();
	});


	for(auto& elem : m_RenderList[RENDER_ALPHA]){
		elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}
	m_RenderList[RENDER_ALPHA].clear();

	return NOERROR;
}

HRESULT CRenderer::RenderUIGroup(_double _dDeltaTime){
	for(auto& elem : m_RenderList[RENDER_PRE_UI]){
			elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}
	m_RenderList[RENDER_PRE_UI].clear();

	for(auto& elem : m_RenderList[RENDER_POST_UI]){
		elem->RenderGameObject(_dDeltaTime);
		SafeRelease(elem);
	}
	m_RenderList[RENDER_POST_UI].clear();

	return NOERROR;
}

HRESULT CRenderer::BlendRenderTarget(){
	if(nullptr == m_pTargetManager
		|| nullptr == m_pBlendShader)
		return E_FAIL;

	m_pTargetManager->SettingOnShader(m_pBlendShader, "g_DiffuseTexture", L"DiffuseTarget");
	m_pTargetManager->SettingOnShader(m_pBlendShader, "g_ShadeTexture", L"ShadeTarget");
	m_pTargetManager->SettingOnShader(m_pBlendShader, "g_SpecularTexture", L"SpecularTarget");


	m_pBlendShader->BeginShader();
	m_pBlendShader->BeginPass(0);

	m_pRenderBuffer->RenderVIBuffer();

	m_pBlendShader->EndPass();
	m_pBlendShader->EndShader();

	return NOERROR;
}

// 프로토타입 생성 함수
CRenderer* CRenderer::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CRenderer* pInstance = new CRenderer(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		MSG_BOX(L"CRenderer Craete Faliure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// 복제 생성 함수
CComponent* CRenderer::CreateClone(void* _pArgument){
	// Renderer는 단 하나만 있으면 되기 때문에 복제 안함
	// 따라서 복제생성 함수에서 원본(프로토타입)을 전달하는 형태로 구현

	// 원본의 레퍼런스 카운터 증가
	AddRef();

	// 원본(프로토타입) 전달
	return this;
}

// Free
void CRenderer::Free(){
	for(size_t i = 0; i < RENDER_END; i++){
		for(auto& elem : m_RenderList[i])
			SafeRelease(elem);
		m_RenderList[i].clear();
	}

	SafeRelease(m_pBlendShader);
	SafeRelease(m_pLightAccShader);
	SafeRelease(m_pRenderBuffer);
	SafeRelease(m_pPipeline);
	SafeRelease(m_pLightManager);
	SafeRelease(m_pTargetManager);

	CComponent::Free();
}