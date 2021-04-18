#include "stdafx.h"
#include "..\Header\StaticMeshObject.h"
#include "..\Header\Client_Headers.h"

USING(Client);

CStaticMeshObject::CStaticMeshObject(LPDIRECT3DDEVICE9 _pGraphicDeivce)
	: CGameObject(_pGraphicDeivce)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pStaticMeshCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_pFrustumCom(nullptr)
	, m_fCullRadius(0.f){
}

CStaticMeshObject::CStaticMeshObject(const CStaticMeshObject & _StaticMeshObject)
	: CGameObject(_StaticMeshObject)
	, m_pRendererCom(_StaticMeshObject.m_pRendererCom)
	, m_pShaderCom(_StaticMeshObject.m_pShaderCom)
	, m_pTransformCom(_StaticMeshObject.m_pTransformCom)
	, m_pStaticMeshCom(_StaticMeshObject.m_pStaticMeshCom)
	, m_pColliderCom(_StaticMeshObject.m_pColliderCom)
	, m_pFrustumCom(_StaticMeshObject.m_pFrustumCom)
	, m_fCullRadius(0.f){
}

HRESULT CStaticMeshObject::InitGameObject_Proto(){
	return NOERROR;
}

HRESULT CStaticMeshObject::InitGameObject_Clone(void* _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	STATIC_MESH_DESC* tDesc = reinterpret_cast<STATIC_MESH_DESC*>(_pArgument);
	
	// ������ ������Ʈ �߰�aw
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// ���̴� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"DefaultShaderCom", L"DefaultShaderCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// Ʈ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, &tDesc->tTransformDesc)))
		return E_FAIL;

	// ����ƽ �޽� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pStaticMeshCom), L"StaticMeshCom", tDesc->szStaticMeshComName, SCENE_TYPE::STAGE_SCENE)))
		return E_FAIL;

	// �ݶ��̴� ũ�� ������� �޽��� ũ�� ���
	_vec3 vMin, vMax;
	if(FAILED(m_pStaticMeshCom->GetBoundaryBox(vMin, vMax)))
		return E_FAIL;

	if(FAILED(m_pStaticMeshCom->GetBoundarySphere(m_vCenter, m_fCullRadius)))
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pGameObj = this;
	ColliderDesc.pParentWorldMatrix = m_pTransformCom->GetWorldMatrixPointer();
	ColliderDesc.vScale = _vec3(fabs(vMax.x - vMin.x), fabs(vMax.y - vMin.y), fabs(vMax.z - vMin.z));
	ColliderDesc.vLocalPosition = _vec3(0.f, fabs(vMax.y - vMin.y)*0.5f, 0.f);

	// �ݶ��̴� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pColliderCom),L"ColliderCom", L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &ColliderDesc)))
		return E_FAIL;

	// ����ü ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pFrustumCom), L"FrustumCom", L"FrustumCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStaticMeshObject::LateInitGameObject(){
	m_pColliderCom->UpdateCollider();

	return NOERROR;
}

HRESULT CStaticMeshObject::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();
	return NOERROR;
}

HRESULT CStaticMeshObject::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom
		|| nullptr == m_pFrustumCom)
		return E_FAIL;

	_matrix worldMatrix = m_pTransformCom->GetWorldMatrix();
	*reinterpret_cast<_vec3*>(worldMatrix.m[3]) += m_vCenter;

	//�������� ��ü ���
	if(m_pFrustumCom->CheckFrustum(&worldMatrix, 2.f))
		m_pRendererCom->AddRenderList(CRenderer::RENDER_NONALPHA, this);

	return NOERROR;
}

HRESULT CStaticMeshObject::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;

	// ���� ��ȯ ���(�׵���ķ� �ʱ�ȭ)
	_matrix matWorld, matView, matProj;
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// ���̴��� ��ü�� ����, ��, ���� ��� ����
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// ī�޶��� ������� ������
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	// ī�޶��� ��ġ
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// ����ƽ �޽��� ����� ���� ����
	_ulong		dwNumSubset = m_pStaticMeshCom->GetSubsetCount();

	// ���̴��� ���Ͽ� ���� ����
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// ����� ������ŭ �ݺ��Ͽ� ����
	for(_ulong i = 0; i < dwNumSubset; ++i){
		// �޽��� ��ǻ�� �ؽ��ĸ� ���̴��� ����
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// �븻���� ������ �븻���� ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// ����ŧ������ ������ ����ŧ������ ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}

		// Begin ���� ���̴��� ���������� ���� �ٲ�ٸ� CommitChange()�Լ� ȣ��
		m_pShaderCom->CommitChange();

		// �޽� ����
		m_pStaticMeshCom->RenderMesh(i);
	}

	// ���̴��� ���� ���� ����
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return NOERROR;
}

CStaticMeshObject * CStaticMeshObject::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce){
	CStaticMeshObject* pInstance = new CStaticMeshObject(_pGraphicDeivce);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMeshObject Create Failure");
	}
	
	return pInstance;
}

CGameObject * CStaticMeshObject::CreateClone(void* _pArgument){
	CGameObject* pInstance = new CStaticMeshObject(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMeshObject Clone Failure");
	}

	return pInstance;
}

void CStaticMeshObject::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pStaticMeshCom);
	SafeRelease(m_pColliderCom);
	SafeRelease(m_pFrustumCom);

	CGameObject::Free();
}
