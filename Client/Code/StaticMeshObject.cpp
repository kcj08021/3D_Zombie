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
	
	// 렌더러 컴포넌트 추가aw
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"DefaultShaderCom", L"DefaultShaderCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 트랜스폼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, &tDesc->tTransformDesc)))
		return E_FAIL;

	// 스태틱 메쉬 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pStaticMeshCom), L"StaticMeshCom", tDesc->szStaticMeshComName, SCENE_TYPE::STAGE_SCENE)))
		return E_FAIL;

	// 콜라이더 크기 계산위해 메쉬의 크기 계산
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

	// 콜라이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pColliderCom),L"ColliderCom", L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &ColliderDesc)))
		return E_FAIL;

	// 절두체 컴포넌트 추가
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

	//렌더러에 객체 등록
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

	// 월드 변환 행렬(항등행렬로 초기화)
	_matrix matWorld, matView, matProj;
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 셰이더에 객체의 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	// 카메라의 위치
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// 스태틱 메쉬의 서브셋 갯수 얻어옴
	_ulong		dwNumSubset = m_pStaticMeshCom->GetSubsetCount();

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// 서브셋 갯수만큼 반복하여 렌더
	for(_ulong i = 0; i < dwNumSubset; ++i){
		// 메쉬의 디퓨즈 텍스쳐를 셰이더에 전달
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// 노말맵이 없으면 노말매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// 스펙큘러맵이 없으면 스펙큘러매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}

		// Begin 이후 셰이더의 전역변수의 값이 바뀐다면 CommitChange()함수 호출
		m_pShaderCom->CommitChange();

		// 메쉬 렌더
		m_pStaticMeshCom->RenderMesh(i);
	}

	// 셰이더를 통한 렌더 종료
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
