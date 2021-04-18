// Client ���
#include "stdafx.h"
#include "..\Header\Terrain.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Terrain.h"
#include "..\Header\UnitManager.h"

// Reference ���
#include "Management.h"

USING(Client)

// ������
CTerrain::CTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pShaderCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pStaticMesh(nullptr)
	, m_pTransformCom(nullptr)
	, m_pPickeeCom(nullptr){
}

// ���� ������
CTerrain::CTerrain(const CTerrain & _Terrain)
	: CGameObject(_Terrain)
	, m_pShaderCom(_Terrain.m_pShaderCom)
	, m_pRendererCom(_Terrain.m_pRendererCom)
	, m_pStaticMesh(_Terrain.m_pStaticMesh)
	, m_pTransformCom(_Terrain.m_pTransformCom)
	, m_pPickeeCom(_Terrain.m_pPickeeCom){
	SafeAddRef(_Terrain.m_pShaderCom);
	SafeAddRef(_Terrain.m_pRendererCom);
	SafeAddRef(_Terrain.m_pTransformCom);
}

HRESULT CTerrain::InitGameObject_Proto(){
	// ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	_matrix matWorld = {};
	D3DXMatrixIdentity(&matWorld);

	CTransform::TRANSFORM_DESC TransformDesc = { &matWorld, 0.f,0.f};

	// Ʈ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, &TransformDesc)))
		return E_FAIL;

	// ���̴� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"TerrainShaderCom", L"DefaultShaderCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	
	return NOERROR;
}

// Initialize with Clone
HRESULT CTerrain::InitGameObject_Clone(void* _pArgument){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pStaticMesh), L"map", L"map_proto", SCENE_TYPE::STAGE_SCENE)))
		return E_FAIL;


	CUnitManager::GetInstance()->SetStageMap(m_pStaticMesh);

	CPickee::PICKEE_DESC PickeeDesc;
	PickeeDesc.pGameObj = this;
	PickeeDesc.pMesh = m_pStaticMesh->GetMesh();
	PickeeDesc.pWolrdMatrix = m_pTransformCom->GetWorldMatrixPointer();

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pPickeeCom), L"PickeeCom", L"PickeeCom_Proto", SCENE_TYPE::STATIC_SCENE, &PickeeDesc)))
		return E_FAIL;

	return NOERROR;
}

// LateInitialize
HRESULT CTerrain::LateInitGameObject(){
	return NOERROR;
}

// Update
HRESULT CTerrain::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	return NOERROR;
}

// LateUpdate
HRESULT CTerrain::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	//�������� ��ü ���
	m_pRendererCom->AddRenderList(CRenderer::RENDER_NONALPHA, this);

	return NOERROR;
}

// Render
HRESULT CTerrain::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pStaticMesh)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;

	// ���� ��ȯ ���(�׵���ķ� �ʱ�ȭ)
	_matrix matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// ī�޶��� ������� ������
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);


	// ���̴��� ����, ��, ���� ��� ����
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// ī�޶��� ��ġ
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// ���̴��� ���Ͽ� ���� ����
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	_uint uiSubsetCount = m_pStaticMesh->GetSubsetCount();

	for(_uint i = 0; i < uiSubsetCount; ++i){
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// �븻���� ������ �븻���� ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// ����ŧ������ ������ ����ŧ������ ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}


		m_pShaderCom->CommitChange();

		m_pStaticMesh->RenderMesh(i);

	}

	// ���̴��� ���� ���� ����
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return hr;
}

// ���� ���� �Լ�(static)
CTerrain * CTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CTerrain* pInstance = new CTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CTerrain Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// ���� ���� �Լ�
CGameObject * CTerrain::CreateClone(void* _pArgument){
	CTerrain* pInstance = new CTerrain(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CTerrain Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// Free
void CTerrain::Free(){
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pStaticMesh);
	SafeRelease(m_pPickeeCom);

	CGameObject::Free();
}
