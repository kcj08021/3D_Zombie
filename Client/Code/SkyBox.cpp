#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\SkyBox.h"

#include "Management.h"

USING(Client)

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 _pGraphicDeivce)
	: CGameObject(_pGraphicDeivce)
	, m_pShaderCom(nullptr)
	, m_pRendereCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr){
}

CSkyBox::CSkyBox(const CSkyBox & _SkyBox)
	: CGameObject(_SkyBox)
	, m_pShaderCom(_SkyBox.m_pShaderCom)
	, m_pRendereCom(_SkyBox.m_pRendereCom)
	, m_pVIBufferCom(_SkyBox.m_pVIBufferCom)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr){
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pRendereCom);
	SafeAddRef(m_pVIBufferCom);
}

HRESULT CSkyBox::InitGameObject_Proto(){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"SkyBoxShader_Proto", STATIC_SCENE)))
		return E_FAIL;
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendereCom), L"RendererCom", L"RendererCom_Proto", STATIC_SCENE)))
		return E_FAIL;
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pVIBufferCom), L"VIBufferCube", L"VIBufferCubeCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkyBox::InitGameObject_Clone(void * _pArgument){
	CTransform::TRANSFORM_DESC tDesc;
	_matrix matWorld = {};
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(-90.f));

	tDesc.fRotationPerSec = 0.f;
	tDesc.fSpeedPerSec = 0.f;
	tDesc.pMatrixWorldSetting = &matWorld;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", STATIC_SCENE, &tDesc)))
		return E_FAIL;

	SKYBOX_DESC* pDesc = reinterpret_cast<SKYBOX_DESC*>(_pArgument);
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTextureCom), L"TextureCom", pDesc->szTextureComTag, STAGE_SCENE)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkyBox::LateInitGameObject(){
	return NOERROR;
}

HRESULT CSkyBox::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

HRESULT CSkyBox::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendereCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	_matrix	ViewMatrix = {};
	pManagement->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, _vec3(ViewMatrix.m[3]));
	
	SafeRelease(pManagement);

	m_pRendereCom->AddRenderList(CRenderer::RENDER_PRE, this);

	return NOERROR;
}

HRESULT CSkyBox::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	CManagement*	pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);


	_matrix matWorld, matView, matProj, matRotate;
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	if(FAILED(m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix))))
		return E_FAIL;
	if(FAILED(m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix))))
		return E_FAIL;

	SafeRelease(pManagement);

	m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture());

	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	m_pVIBufferCom->RenderVIBufferCube();

	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return NOERROR;
}

CSkyBox * CSkyBox::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce){
	CSkyBox* pInstance = new CSkyBox(_pGraphicDeivce);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CSkyBox Create Failure");
	}
	return pInstance;
}

CGameObject * CSkyBox::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CSkyBox(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CSkyBox Clone Failure");
	}

	return pInstance;
}

void CSkyBox::Free(){
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pRendereCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pVIBufferCom);

	CGameObject::Free();
}
