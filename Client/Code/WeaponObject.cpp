#include "stdafx.h"
#include "..\Header\WeaponObject.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Weapon.h"

USING(Client)

CWeaponObject::CWeaponObject(LPDIRECT3DDEVICE9 _pGraphicDeive)
	: CGameObject(_pGraphicDeive)
	, m_pWeapon(nullptr)
	, m_pCollider(nullptr)
	, m_pTransformCom(nullptr)
	, m_pRenderer(nullptr)
	, m_pShaderCom(nullptr)
	, m_pStaticMeshStageMap(nullptr)
	, m_fGravityAcc(0.f)
	, m_fDeadTime(0.f)
	, m_bIsGround(false){
}

CWeaponObject::CWeaponObject(const CWeaponObject & _WeaponObject)
	: CGameObject(_WeaponObject)
	, m_pWeapon(nullptr)
	, m_pCollider(nullptr)
	, m_pTransformCom(nullptr)
	, m_pRenderer(_WeaponObject.m_pRenderer)
	, m_pShaderCom(_WeaponObject.m_pShaderCom)
	, m_pStaticMeshStageMap(_WeaponObject.m_pStaticMeshStageMap)
	, m_fGravityAcc(0.f)
	, m_fDeadTime(0.f)
	, m_bIsGround(false){
	SafeAddRef(m_pRenderer);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pStaticMeshStageMap);
}

HRESULT CWeaponObject::InitGameObject_Proto(CStaticMesh* _pStaticMeshStageMap){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"DefaultShaderCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRenderer), L"RendererCom", L"RendererCom_Proto", STATIC_SCENE)))
		return E_FAIL;

	if(nullptr == _pStaticMeshStageMap)
		return E_FAIL;
	
	m_pStaticMeshStageMap = _pStaticMeshStageMap;
	SafeAddRef(m_pStaticMeshStageMap);

	return NOERROR;
}

HRESULT CWeaponObject::InitGameObject_Clone(void * _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	WEAPONOBJECT_DESC tDest = *reinterpret_cast<WEAPONOBJECT_DESC*>(_pArgument);

	m_pWeapon = tDest.pWeapon;
	m_pWeapon->SetThirdPerson(true);
	m_pWeapon->SetPlayerFlag(nullptr);
	m_pWeapon->SetPlayerPicker(nullptr);
	m_pWeapon->SetLateInit(false);

	_matrix matScale;
	D3DXMatrixScaling(&matScale, 0.6f, 0.6f, 0.6f);

	CTransform::TRANSFORM_DESC tTransformDesc;
	tTransformDesc.fRotationPerSec = 0.f;
	tTransformDesc.fSpeedPerSec = 0.f;
	tTransformDesc.pMatrixWorldSetting = &(matScale * (*tDest.pWorldMatrix));

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &tTransformDesc)))
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pGameObj = this;
	ColliderDesc.pParentWorldMatrix = m_pTransformCom->GetWorldMatrixPointer();
	ColliderDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	ColliderDesc.vScale = _vec3(1.f, 1.f, 1.f);

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pCollider), L"ColliderCom", L"Sphere_Collider_Proto", STATIC_SCENE, &ColliderDesc)))
		return E_FAIL;

	if(FAILED(m_pCollider->SettingToCollisionManager()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponObject::LateInitGameObject(){
	return NOERROR;
}

HRESULT CWeaponObject::UpdateGameObject(_double _dDeltaTime){
	if(m_bIsLateInit)
		CGameObject::LateInitGameObject();

	m_fDeadTime += _float(_dDeltaTime);

	if(!m_bIsGround)
		LandingGround(_dDeltaTime);

	return NOERROR;
}

HRESULT CWeaponObject::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRenderer)
		return E_FAIL;

	if(m_fDeadTime > 30.f){
		m_pCollider->DeleteToCollisionManager();

		m_bIsDeadObject = true;
	}
	m_pCollider->UpdateCollider();

	m_pRenderer->AddRenderList(CRenderer::RENDER_NONALPHA, this);

	return NOERROR;
}

HRESULT CWeaponObject::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pWeapon)
		return E_FAIL;

	CDynamicMesh* pWeaponMesh = m_pWeapon->GetThirdPersonModelWeapon();
	if(nullptr == pWeaponMesh)
		return E_FAIL;

	if(FAILED(SettingShader()))
		return E_FAIL;


	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	_uint uiMeshContainerCount = pWeaponMesh->GetMeshContainerCount();

	for(_uint i = 0; i < uiMeshContainerCount; ++i){
		_uint uiSubsetCount = pWeaponMesh->GetSubsetCount(i);

		for(_uint j = 0; j < uiSubsetCount; ++j){
			LPDIRECT3DTEXTURE9 Texture = pWeaponMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_DIFFUSE);
			m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

			Texture = pWeaponMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_NORMAL);
			// 노말맵이 없으면 노말매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsNormalMapping", true);
				m_pShaderCom->SetTexture("g_NormalTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsNormalMapping", false);
			}

			Texture = pWeaponMesh->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_SPECULAR);
			// 스펙큘러맵이 없으면 스펙큘러매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
				m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
			}

			m_pShaderCom->CommitChange();

			pWeaponMesh->RenderMesh(i, j);
		}
	}

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	return NOERROR;
}

HRESULT CWeaponObject::ChangeWeapon(CWeapon ** _pWeapon){
	if(nullptr == _pWeapon
		&& nullptr == m_pWeapon)
		return E_FAIL;

	(*_pWeapon)->SetThirdPerson(true);
	(*_pWeapon)->AnimationReset();
	(*_pWeapon)->SetPlayerFlag(nullptr);
	(*_pWeapon)->SetPlayerPicker(nullptr);
	(*_pWeapon)->SetLateInit(false);
	
	m_pWeapon->SetThirdPerson(false);
	m_pWeapon->SetLateInit(false);
	m_pWeapon->AnimationReset();

	CWeapon* pTempWeapon = *_pWeapon;
	*_pWeapon = m_pWeapon;
	m_pWeapon = pTempWeapon;

	m_fDeadTime = 0.f;


	return NOERROR;
}

const _tchar * CWeaponObject::GetName(){
	return m_pWeapon->GetName();
}

HRESULT CWeaponObject::SettingShader(){
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);
	
	// 카메라의 위치
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// 각 변환행렬 변수들
	_matrix matWorld, matView, matProj;

	// 뷰 변환 행렬과 투영 행렬
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 셰이더에 객체의 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	SafeRelease(pManagement);

	return NOERROR;
}

void CWeaponObject::LandingGround(_double _dDeltaTime){
	LPD3DXMESH pMesh = m_pStaticMeshStageMap->GetMesh();
	if(nullptr == pMesh)
		return;

	_vec3 vRayPos, vPos;
	vRayPos = vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vec3 vRayDir = { 0.f, -1.f, 0.f };

	BOOL bHit = false;
	DWORD dwFaceIndex = 0;

	_float fU, fV, fDist;

	LPD3DXBUFFER pAllHit = nullptr;

	DWORD dwAllHitCount = 0;

	D3DXIntersect(pMesh, &vRayPos, &vRayDir, &bHit, &dwFaceIndex, &fU, &fV, &fDist, &pAllHit, &dwAllHitCount);

	if(bHit){
		vRayPos += (vRayDir * fDist);

		if(vRayPos.y+0.1f + GRAVITY_ACCEL(_dDeltaTime) < vPos.y){
			m_fGravityAcc += GRAVITY_ACCEL(_dDeltaTime);
			vPos.y -= m_fGravityAcc;
			if(vPos.y < vRayPos.y + 0.1f){
				_matrix* matWorld = m_pTransformCom->GetWorldMatrixPointer();
				_matrix matRotate;
				D3DXMatrixRotationX(&matRotate, D3DXToRadian(90.f));
				*matWorld *= matRotate;
				vPos.y = vRayPos.y + 0.1f;
				m_fGravityAcc = 0.f;
				m_bIsGround = true;
			}
		}
	}

	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
}

CWeaponObject * CWeaponObject::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CStaticMesh* _pStaticMeshStageMap){
	CWeaponObject* pInstance = new CWeaponObject(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pStaticMeshStageMap))){
		MSG_BOX(L"CWeaponObject Create Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

CGameObject * CWeaponObject::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CWeaponObject(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CWeaponObject Clone Failure");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CWeaponObject::Free(){
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pRenderer);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pCollider);
	SafeRelease(m_pWeapon);
	SafeRelease(m_pStaticMeshStageMap);

	CGameObject::Free();
}
