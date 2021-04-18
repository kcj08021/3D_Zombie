#include "stdafx.h"
// 클라이언트 헤더
#include "..\Header\Client_Headers.h"
#include "..\Header\Background.h"

// Reference 헤더
#include "Management.h"

USING(Client)

// 생성자
CBackground::CBackground(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pVertexBufferCom(nullptr)
	, m_pTextureCom(nullptr){
}

// 복사 생성자
CBackground::CBackground(const CBackground & _pObj)
	: CGameObject(_pObj){
}
// Initialize with Prototype
HRESULT CBackground::InitGameObject_Proto(){
	return NOERROR;
}

// Initialize with Clone
HRESULT CBackground::InitGameObject_Clone(void* _pArgument){
	// 렌더러 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"UIShader_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 버텍스 버퍼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pVertexBufferCom), L"VIBufferRectCom", L"VIBufferRectCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 텍스쳐 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTextureCom), L"TextureCom", L"TextureCom_Proto", SCENE_TYPE::TITLE_SCENE)))
		return E_FAIL;

	return NOERROR;
}

// LateInitailize
HRESULT CBackground::LateInitGameObject(){
	return NOERROR;
}

// Update
HRESULT CBackground::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	return NOERROR;
}

// LateUpdate
HRESULT CBackground::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	//렌더러에 객체 등록
	m_pRendererCom->AddRenderList(CRenderer::RENDER_UI, this);

	return NOERROR;
}

// Render
HRESULT CBackground::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pVertexBufferCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	HRESULT hr = 0;

	// 월드 변환 행렬(항등행렬로 초기화)
	_matrix matWorld = {};
	D3DXMatrixIdentity(&matWorld);

	// 셰이더에 월드 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matWorld, sizeof(_matrix));

	// 셰이더에 텍스쳐 전달
	m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture());
	
	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// 컴포넌트에 렌더시작 알림
	hr = m_pVertexBufferCom->RenderVIBufferRect();

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return hr;
}

// Prototype 생성 함수(static)
CBackground * CBackground::CreatePrototype(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CBackground* pInstance = new CBackground(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CBackground Craete Faliure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Clone 생성 함수
CGameObject * CBackground::CreateClone(void* _pArgument){
	CBackground* pInstance = new CBackground(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CBackground Clone Craete Faliure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Free
void CBackground::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVertexBufferCom);
	SafeRelease(m_pTextureCom);

	CGameObject::Free();
}

