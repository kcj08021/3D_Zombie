#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Grenade.h"
#include "..\Header\NonPlayer.h"

#include "Management.h"

USING(Client)

CGrenade::CGrenade(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pPicker(nullptr)
	, m_pGrenadeMeshCom(nullptr)
	, m_pFrustum(nullptr)
	, m_fSpeed(0.f){
	::ZeroMemory(m_vDir, sizeof(_vec3));
}

CGrenade::CGrenade(const CGrenade & _Grenade)
	: CGameObject(_Grenade)
	, m_pRendererCom(_Grenade.m_pRendererCom)
	, m_pShaderCom(_Grenade.m_pShaderCom)
	, m_pTransformCom(_Grenade.m_pTransformCom)
	, m_pPicker(_Grenade.m_pPicker)
	, m_pGrenadeMeshCom(_Grenade.m_pGrenadeMeshCom)
	, m_pFrustum(_Grenade.m_pFrustum)
	, m_vDir(_Grenade.m_vDir)
	, m_fSpeed(0.f){
	SafeAddRef(m_pRendererCom);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pTransformCom);
	SafeAddRef(m_pPicker);
	SafeAddRef(m_pGrenadeMeshCom);
	SafeAddRef(m_pFrustum);
}

CGrenade::~CGrenade(){
}

HRESULT CGrenade::InitGameObject_Proto(){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"DefaultShaderCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pPicker), L"PickerCom", L"PickerCom_Proto", STATIC_SCENE)))
		return E_FAIL;
	
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pFrustum), L"FrustumCom", L"FrustumCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGrenade::InitGameObject_Clone(void * _pArgument){
	_matrix matWorld = reinterpret_cast<GRENADE_DESC*>(_pArgument)->matWorld;
	m_eType = reinterpret_cast<GRENADE_DESC*>(_pArgument)->eType;

	*reinterpret_cast<_vec3*>(matWorld.m[3]) += m_vDir = *reinterpret_cast<_vec3*>(matWorld.m[2])*(1.f + 0.5f*m_eType);
	CTransform::TRANSFORM_DESC tDesc;
	tDesc.fRotationPerSec = D3DXToRadian(180.f);
	tDesc.fSpeedPerSec = m_fSpeed = (m_eType == HAND) ? 20.f : 40.f;
	tDesc.pMatrixWorldSetting = &matWorld;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", STATIC_SCENE, &tDesc)))
		return E_FAIL;


	if(m_eType == HAND){
		m_fRange = 3.f;
		if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pGrenadeMeshCom), L"StaticMeshCom", L"projectile_m67_grenade_Proto", STAGE_SCENE)))
			return E_FAIL;
	}
	else if(m_eType == LAUNCHER){
		m_fRange = 2.f;
		if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pGrenadeMeshCom), L"StaticMeshCom", L"projectile_m203_grenade_Proto", STAGE_SCENE)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CGrenade::LateInitGameObject(){
	return NOERROR;
}

HRESULT CGrenade::UpdateGameObject(_double _dDeltaTime){
	if(m_bIsLateInit)
		CGameObject::LateInitGameObject();

	_vec3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vec3 vHit = {};

	m_pPicker->CheckPicking(m_vDir, vPos, &vHit);

	if(D3DXVec3Length(&(m_vDir*m_fSpeed))* _dDeltaTime > D3DXVec3Length(&(vHit - vPos))){
		if(!FAILED(SetExplosion())){
			m_bIsDeadObject = true;
			return NOERROR;
		}
	}

	m_pTransformCom->Move(_dDeltaTime, m_vDir);
	if(m_eType == HAND || m_eType == FLASH)
		m_pTransformCom->RotationAxis(&_vec3(-1.f, 0.f, 0.f), _dDeltaTime);
	else
		m_pTransformCom->RotationAxis(&m_pTransformCom->GetState(CTransform::STATE_LOOK), _dDeltaTime);

	if(m_vDir.y >= -1.f){
		m_vDir.y -= GRAVITY_ACCEL(_dDeltaTime);
		D3DXVec3Normalize(&m_vDir, &m_vDir);
	}
	return NOERROR;
}

HRESULT CGrenade::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom
		|| nullptr == m_pFrustum)
		return E_FAIL;

	if(m_pFrustum->CheckFrustum(m_pTransformCom->GetWorldMatrixPointer(), 0.5f))
		m_pRendererCom->AddRenderList(CRenderer::RENDER_NONALPHA, this);

	return NOERROR;
}

HRESULT CGrenade::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pTransformCom
		|| nullptr == m_pGrenadeMeshCom)
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
	_ulong dwNumSubset = m_pGrenadeMeshCom->GetSubsetCount();

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// 서브셋 갯수만큼 반복하여 렌더
	for(_ulong i = 0; i < dwNumSubset; ++i){
		// 메쉬의 디퓨즈 텍스쳐를 셰이더에 전달
		LPDIRECT3DTEXTURE9 Texture = m_pGrenadeMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pGrenadeMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// 노말맵이 없으면 노말매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pGrenadeMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
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
		m_pGrenadeMeshCom->RenderMesh(i);
	}

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CGrenade::SetExplosion(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	list<CGameObject*>* pList = pManagement->GetObjectList(STAGE_SCENE, L"EnemyLayer");
	if(nullptr != pList){
		for(auto& elem : *pList){
			_vec3 vPos = dynamic_cast<CNonPlayer*>(elem)->GetPosition();
			_vec3 vDir = m_pTransformCom->GetState(CTransform::STATE_POSITION) - vPos;
			if(m_fRange > D3DXVec3Length(&(vDir))){
				dynamic_cast<CNonPlayer*>(elem)->SetExplosionDead(D3DXVec3Normalize(&vDir, &vDir));
			}
		}
	}

	pList = pManagement->GetObjectList(STAGE_SCENE, L"AllieLayer");
	if(nullptr != pList){

	}

	pList = pManagement->GetObjectList(STAGE_SCENE, L"PlayerLayer");
	if(nullptr != pList){

	}

	SafeRelease(pManagement);

	return NOERROR;
}

CGrenade * CGrenade::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CGrenade* pInstance = new CGrenade(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CGrenade Create Failure");
	}
	return pInstance;
}

CGameObject * CGrenade::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CGrenade(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CGrenade Clone Failure");
	}
	return pInstance;
}

void CGrenade::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pGrenadeMeshCom);
	SafeRelease(m_pPicker);
	SafeRelease(m_pFrustum);


	CGameObject::Free();
}
