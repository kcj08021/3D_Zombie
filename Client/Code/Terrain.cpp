// Client 헤더
#include "stdafx.h"
#include "..\Header\Terrain.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Terrain.h"
#include "..\Header\UnitManager.h"

// Reference 헤더
#include "Management.h"

USING(Client)

// 생성자
CTerrain::CTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pShaderCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pStaticMesh(nullptr)
	, m_pTransformCom(nullptr)
	, m_pPickeeCom(nullptr){
}

// 복사 생성자
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
	// 렌더러 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	_matrix matWorld = {};
	D3DXMatrixIdentity(&matWorld);

	CTransform::TRANSFORM_DESC TransformDesc = { &matWorld, 0.f,0.f};

	// 트랜스폼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, &TransformDesc)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
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

	//렌더러에 객체 등록
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

	// 월드 변환 행렬(항등행렬로 초기화)
	_matrix matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);


	// 셰이더에 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// 카메라의 위치
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	_uint uiSubsetCount = m_pStaticMesh->GetSubsetCount();

	for(_uint i = 0; i < uiSubsetCount; ++i){
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// 노말맵이 없으면 노말매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMesh->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// 스펙큘러맵이 없으면 스펙큘러매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}


		m_pShaderCom->CommitChange();

		m_pStaticMesh->RenderMesh(i);

	}

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return hr;
}

// 원본 생성 함수(static)
CTerrain * CTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CTerrain* pInstance = new CTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		MSG_BOX(L"CTerrain Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

// 복제 생성 함수
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
