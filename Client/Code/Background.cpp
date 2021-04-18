#include "stdafx.h"
// Ŭ���̾�Ʈ ���
#include "..\Header\Client_Headers.h"
#include "..\Header\Background.h"

// Reference ���
#include "Management.h"

USING(Client)

// ������
CBackground::CBackground(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pVertexBufferCom(nullptr)
	, m_pTextureCom(nullptr){
}

// ���� ������
CBackground::CBackground(const CBackground & _pObj)
	: CGameObject(_pObj){
}
// Initialize with Prototype
HRESULT CBackground::InitGameObject_Proto(){
	return NOERROR;
}

// Initialize with Clone
HRESULT CBackground::InitGameObject_Clone(void* _pArgument){
	// ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// ���̴� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"UIShader_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// ���ؽ� ���� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pVertexBufferCom), L"VIBufferRectCom", L"VIBufferRectCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// �ؽ��� ������Ʈ �߰�
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

	//�������� ��ü ���
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

	// ���� ��ȯ ���(�׵���ķ� �ʱ�ȭ)
	_matrix matWorld = {};
	D3DXMatrixIdentity(&matWorld);

	// ���̴��� ���� ��� ����
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matWorld, sizeof(_matrix));

	// ���̴��� �ؽ��� ����
	m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture());
	
	// ���̴��� ���Ͽ� ���� ����
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// ������Ʈ�� �������� �˸�
	hr = m_pVertexBufferCom->RenderVIBufferRect();

	// ���̴��� ���� ���� ����
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return hr;
}

// Prototype ���� �Լ�(static)
CBackground * CBackground::CreatePrototype(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CBackground* pInstance = new CBackground(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CBackground Craete Faliure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

// Clone ���� �Լ�
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

