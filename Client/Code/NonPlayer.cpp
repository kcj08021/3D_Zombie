#include "stdafx.h"
#include "..\Header\NonPlayer.h"

#include "..\Header\StaticMeshObject.h"
#include "..\Header\Weapon.h"
#include "..\Header\UnitManager.h"
#include "..\Header\BloodEffect.h"
#include "..\Header\WeaponObject.h"
#include "Management.h"

USING(Client)

// 생성자
CNonPlayer::CNonPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pFrustumCom(nullptr)
	, m_pStaticMeshMap(nullptr)
	, m_pDynamicMeshCom(nullptr)
	, m_pPickeeCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pNavigationCom(nullptr)
	, m_bIsDead(false)
	, m_bAttackCheck(false)
	, m_fHP(c_fZombieFullHP){
	::ZeroMemory(m_PartWorldMatrx, sizeof(_matrix) * PART_END);
	::ZeroMemory(m_pPartCombinedMatrix, sizeof(_matrix*) * PART_END);
	::ZeroMemory(m_pPartColliderCom, sizeof(_matrix*) * PART_END);
}

// 복사 생성자
CNonPlayer::CNonPlayer(const CNonPlayer & _NonPlayer)
	: CGameObject(_NonPlayer)
	, m_pRendererCom(_NonPlayer.m_pRendererCom)
	, m_pShaderCom(_NonPlayer.m_pShaderCom)
	, m_pFrustumCom(_NonPlayer.m_pFrustumCom)
	, m_pStaticMeshMap(_NonPlayer.m_pStaticMeshMap)
	, m_pDynamicMeshCom(nullptr)
	, m_pPickeeCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_bAttackCheck(false)
	, m_fHP(c_fZombieFullHP){
	SafeAddRef(m_pRendererCom);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pFrustumCom);
	SafeAddRef(m_pStaticMeshMap);
	::ZeroMemory(m_PartWorldMatrx, sizeof(_matrix) * PART_END);
	::ZeroMemory(m_pPartCombinedMatrix, sizeof(_matrix*) * PART_END);
	::ZeroMemory(m_pPartColliderCom, sizeof(_matrix*) * PART_END);
}

// Initialize with Prototype
HRESULT CNonPlayer::InitGameObject_Proto(CStaticMeshObject* _pStaticMesh){
	// 렌더러 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"DefaultShaderCom", L"DefaultShaderCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	// 절두체 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pFrustumCom), L"FrustumCom", L"FrustumCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	if(nullptr != _pStaticMesh){
		m_pStaticMeshMap = _pStaticMesh;
		SafeAddRef(_pStaticMesh);
	}

	return NOERROR;
}

// Initialize with Clone
HRESULT CNonPlayer::InitGameObject_Clone(void * _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	NONPLAYER_DESC* tDesc = reinterpret_cast<NONPLAYER_DESC*>(_pArgument);

	// 트랜스폼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", SCENE_TYPE::STATIC_SCENE, &tDesc->tTransformDesc)))
		return E_FAIL;

	// 다이나믹 메쉬 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pDynamicMeshCom), L"DynamicMeshCom", tDesc->szDynamicMeshProtoName, SCENE_TYPE::STAGE_SCENE)))
		return E_FAIL;

	SettingPart();

	CPickee::PICKEE_DESC tPickeeDest = {};
	tPickeeDest.pGameObj = this;
	tPickeeDest.pMesh = m_pPartColliderCom[PARENT]->GetMesh();
	tPickeeDest.pWolrdMatrix = m_pPartColliderCom[PARENT]->GetWorldMatrixPointer();

	// 피킹 대상을 위한 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pPickeeCom), L"PickeeCom", L"PickeeCom_Proto", SCENE_TYPE::STATIC_SCENE, &tPickeeDest)))
		return E_FAIL;

	// 네비게이션 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pNavigationCom), L"NavigationCom", L"Navigation_Proto", SCENE_TYPE::STAGE_SCENE, &(m_pTransformCom->GetState(CTransform::STATE_POSITION)))))
		return E_FAIL;


	return NOERROR;
}

// LateInitialize
HRESULT CNonPlayer::LateInitGameObject(){
	CManagement* pManagement = CManagement::GetInstance();
	if(pManagement != nullptr){
		SafeAddRef(pManagement);
		m_pBloodEffect = reinterpret_cast<CBloodEffect*>((pManagement->GetObjectList(STAGE_SCENE, L"BloodEffect")->front()));

		SafeRelease(pManagement);
	}

	m_bIsLateInit = true;

	return NOERROR;
}

// Update
HRESULT CNonPlayer::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	if(m_bIsDead || m_eCurrAnimation == ZOMBIE_DEATH){
		m_fDeadTime += (_float)_dDeltaTime;
		return NOERROR;
	}

	// 플레이어 위치 가져오기
	CUnitManager* pUnitManager = CUnitManager::GetInstance();
	if(nullptr == pUnitManager)
		return E_FAIL;
	SafeAddRef(pUnitManager);

	_float fPlayerDistance = D3DXVec3Length(&(pUnitManager->GetPlayerPos() - (m_pTransformCom->GetState(CTransform::STATE_POSITION) + _vec3(0.f, 1.75f, 0.f))));

	if(fPlayerDistance < 1.2f || m_eCurrAnimation == ZOMBIE_ATTACK){
		Attack();
	} else
		//if(m_eCurrAnimation != ANIMATION_SET::ZOMBIE_DEATH)
		CharacterMove(_dDeltaTime);

	// 스테이지 지면에 붙어있도록 함
	if(nullptr != pUnitManager){
		_vec3 vRayPos, vPos;
		vRayPos = vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
		vPos.y += 2.0;	// 임의로 설정된 높이
		_vec3 vRayDir = { 0.f, -1.f, 0.f };
		_float fDist = 0.f;

		pUnitManager->IntersectRayToStageMap(vRayDir, vPos, &fDist);

		vPos.y -= fDist;

		m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
	}

	SafeRelease(pUnitManager);

	return NOERROR;
}

// LateUpdate
HRESULT CNonPlayer::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pFrustumCom
		|| nullptr == m_pRendererCom
		|| nullptr == m_pTransformCom)
		return E_FAIL;

	if(m_fDeadTime > 10.f){
		for(_uint i = 0; i < PART_END; ++i)
			m_pPartColliderCom[i]->DeleteToCollisionManager();
		m_bIsDeadObject = true;
	}
	
	_matrix CullMatrix = m_pTransformCom->GetWorldMatrix();

	CullMatrix.m[3][1] += 1.f;

	CheckState();

	//절두체 컬링 체크
	if(!m_pFrustumCom->CheckFrustum(&CullMatrix, 2.f))
		return NOERROR;

	m_pRendererCom->AddRenderList(CRenderer::RENDER_NONALPHA, this);

	return NOERROR;
}

// Render
HRESULT CNonPlayer::RenderGameObject(_double _dDeltaTime){
	// 같은 메쉬를 공유해서 이렇게 밖에 안되나?
	m_pDynamicMeshCom->PlayAnimationSet(_dDeltaTime * !m_bIsDead);

	UpdatePart();


	// 셰이더의 전역변수 설정
	if(FAILED(SettingShader()))
		return E_FAIL;

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	_uint uiMeshContainerCount = m_pDynamicMeshCom->GetMeshContainerCount();

	for(size_t i = 0; i < uiMeshContainerCount; ++i){
		_ulong dwSubsetCount = m_pDynamicMeshCom->GetSubsetCount(i);

		m_pDynamicMeshCom->UpdateSkinnedMesh(i);

		for(_ulong j = 0; j < dwSubsetCount; ++j){
			LPDIRECT3DTEXTURE9 Texture = m_pDynamicMeshCom->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_DIFFUSE);
			m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

			Texture = m_pDynamicMeshCom->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_NORMAL);
			// 노말맵이 없으면 노말매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsNormalMapping", true);
				m_pShaderCom->SetTexture("g_NormalTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsNormalMapping", false);
			}

			Texture = m_pDynamicMeshCom->GetMaterialTexture(i, j, MESH_TEXTURE::TYPE_SPECULAR);
			// 스펙큘러맵이 없으면 스펙큘러매핑 끔
			if(nullptr != Texture){
				m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
				m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
			} else{
				m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
			}

			m_pShaderCom->CommitChange();

			m_pDynamicMeshCom->RenderMesh(i, j);
		}
	}

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

#ifdef _DEBUG
	// RenderPart();
#endif // _DEBUG


	return NOERROR;
}

HRESULT CNonPlayer::SetExplosionDead(_vec3 * vDir){
	_vec3 vLook = _vec3(m_pPartCombinedMatrix[PARENT]->m[2]);
	D3DXVec3Normalize(&vLook, &vLook);

	SetDeath();

	return NOERROR;
}

_bool CNonPlayer::CheckHit(const _vec3 & _vRayDir, const _vec3 & _vRayPos, _float _fDamage){
	if(m_bIsDead == true)
		return false;

	_bool bIsHit = false;

	for(_uint i = HEAD; i < PART_END; ++i){
		_vec3 vRayDir, vRayPos;
		_matrix matWorldInv = {};

		D3DXMatrixInverse(&matWorldInv, nullptr, m_pPartColliderCom[i]->GetWorldMatrixPointer());

		D3DXVec3TransformNormal(&vRayDir, &_vRayDir, &matWorldInv);
		D3DXVec3TransformCoord(&vRayPos, &_vRayPos, &matWorldInv);

		BOOL bHit = FALSE;
		_float fU = 0.f, fV = 0.f, fDist = 0.f;
		DWORD dwFaceIndex = 0;
		LPD3DXBUFFER pAllHitBuffer = nullptr;
		DWORD dwHitCount = 0;

		D3DXIntersect(m_pPartColliderCom[i]->GetMesh(), &vRayPos, &vRayDir, &bHit, &dwFaceIndex, &fU, &fV, &fDist, &pAllHitBuffer, &dwHitCount);
		if(TRUE == bHit){
			if(nullptr != m_pBloodEffect){
				_vec3 vEffectPos = _vRayPos + (_vRayDir * (fDist - (m_eCurrAnimation == ZOMBIE_WALK ? 2.f : 1.f)));
				m_pBloodEffect->SetEffectPosition(vEffectPos);
			}
			m_fHP -= _fDamage;
			break;
		}
	}
	if(m_fHP < 0.f)
		SetDeath();

	return true;
}

HRESULT CNonPlayer::SettingPart(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	/////////////////////////////////////////////////////////////////////////
	// 부모격 콜라이더
	D3DXFRAME_DERIVED* pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_tag_origin");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[PARENT] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[PARENT] = *m_pPartCombinedMatrix[PARENT] * m_pTransformCom->GetWorldMatrix();

	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.pGameObj = this;
	CollDesc.vScale = _vec3(1.f, 2.f, 1.f);
	CollDesc.vLocalPosition = _vec3(0.f, 1.f, 0.f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[PARENT];

	m_pPartColliderCom[PARENT] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	/////////////////////////////////////////////////////////////////////////
	// 머리 콜라이더
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_head");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[HEAD] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[HEAD] = *m_pPartCombinedMatrix[HEAD] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.3f, 0.3f, 0.3f);
	CollDesc.vLocalPosition = _vec3(-0.03f, 0.06f, 0.f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[HEAD];

	m_pPartColliderCom[HEAD] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"Sphere_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	/////////////////////////////////////////////////////////////////////////
	// 윗몸통 콜라이더
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_spine4");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[BODY_UPPER] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[BODY_UPPER] = *m_pPartCombinedMatrix[BODY_UPPER] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.55f, 0.4f, 0.3f);
	CollDesc.vLocalPosition = _vec3(-0.05f, 0.f, 0.f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[BODY_UPPER];

	m_pPartColliderCom[BODY_UPPER] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	/////////////////////////////////////////////////////////////////////////
	// 아랫몸통 콜라이더
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_mainroot");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[BDOY_LOWER] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[BDOY_LOWER] = *m_pPartCombinedMatrix[BDOY_LOWER] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.3f, 0.4f, 0.3f);
	CollDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[BDOY_LOWER];

	m_pPartColliderCom[BDOY_LOWER] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));


	/////////////////////////////////////////////////////////////////////////
	// 왼쪽 어깨
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_shoulder_le");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[LEFT_SHOULDER] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[LEFT_SHOULDER] = *m_pPartCombinedMatrix[LEFT_SHOULDER] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.38f, 0.12f, 0.15f);
	CollDesc.vLocalPosition = _vec3(0.15f, 0.f, 0.03f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[LEFT_SHOULDER];

	m_pPartColliderCom[LEFT_SHOULDER] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));


	/////////////////////////////////////////////////////////////////////////
	// 왼쪽 팔
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_elbow_le");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[LEFT_ELBOW] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[LEFT_ELBOW] = *m_pPartCombinedMatrix[LEFT_ELBOW] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.38f, 0.12f, 0.15f);
	CollDesc.vLocalPosition = _vec3(0.13f, 0.f, 0.02f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[LEFT_ELBOW];

	m_pPartColliderCom[LEFT_ELBOW] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));


	/////////////////////////////////////////////////////////////////////////
	// 오른쪽 어깨
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_shoulder_ri");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[RIGHT_SHOULDER] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[RIGHT_SHOULDER] = *m_pPartCombinedMatrix[RIGHT_SHOULDER] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.38f, 0.12f, 0.15f);
	CollDesc.vLocalPosition = _vec3(-0.15f, 0.f, -0.03f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[RIGHT_SHOULDER];

	m_pPartColliderCom[RIGHT_SHOULDER] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	/////////////////////////////////////////////////////////////////////////
	// 오른쪽 팔
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_elbow_ri");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[RIGHT_ELBOW] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[RIGHT_ELBOW] = *m_pPartCombinedMatrix[RIGHT_ELBOW] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.38f, 0.12f, 0.15f);
	CollDesc.vLocalPosition = _vec3(-0.13f, 0.f, -0.02f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[RIGHT_ELBOW];

	m_pPartColliderCom[RIGHT_ELBOW] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));
	m_pPartColliderCom[RIGHT_ELBOW]->SettingToCollisionManager();
	/////////////////////////////////////////////////////////////////////////
	// 왼쪽 허벅지
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_hip_le");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[LFET_HIP] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[LFET_HIP] = *m_pPartCombinedMatrix[LFET_HIP] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.7f, 0.20f, 0.22f);
	CollDesc.vLocalPosition = _vec3(0.2f, 0.f, 0.02f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[LFET_HIP];

	m_pPartColliderCom[LFET_HIP] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));


	/////////////////////////////////////////////////////////////////////////
	// 왼쪽 종아리
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_knee_le");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[LEFT_KNEE] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[LEFT_KNEE] = *m_pPartCombinedMatrix[LEFT_KNEE] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.55f, 0.17f, 0.17f);
	CollDesc.vLocalPosition = _vec3(0.25f, 0.f, 0.01f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[LEFT_KNEE];

	m_pPartColliderCom[LEFT_KNEE] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	///////////////////////////////////////////////////////////////////////////
	// 오른쪽 허벅지
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_hip_ri");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[RIGHT_HIP] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[RIGHT_HIP] = *m_pPartCombinedMatrix[RIGHT_HIP] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.7f, 0.20f, 0.22f);
	CollDesc.vLocalPosition = _vec3(-0.2f, 0.f, -0.01f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[RIGHT_HIP];

	m_pPartColliderCom[RIGHT_HIP] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	/////////////////////////////////////////////////////////////////////////
	// 오른쪽 종아리
	pFrame = m_pDynamicMeshCom->GetFrame("body_sp_arab_yasir_j_knee_ri");
	if(nullptr == pFrame){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pPartCombinedMatrix[RIGHT_KNEE] = &pFrame->CombinedTransformationMatrix;
	m_PartWorldMatrx[RIGHT_KNEE] = *m_pPartCombinedMatrix[RIGHT_KNEE] * m_pTransformCom->GetWorldMatrix();

	CollDesc = {};
	CollDesc.vScale = _vec3(0.55f, 0.17f, 0.17f);
	CollDesc.vLocalPosition = _vec3(-0.25f, 0.f, -0.02f);
	CollDesc.pParentWorldMatrix = &m_PartWorldMatrx[RIGHT_KNEE];

	m_pPartColliderCom[RIGHT_KNEE] = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc));

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CNonPlayer::UpdatePart(){
	for(_uint i = 0; i < PART_END; ++i){
		if(nullptr == m_pPartCombinedMatrix[i]
			|| nullptr == m_pPartColliderCom[i])
			continue;

		m_PartWorldMatrx[i] = *m_pPartCombinedMatrix[i] * m_pTransformCom->GetWorldMatrix();

		m_pPartColliderCom[i]->UpdateCollider();

	}

	return NOERROR;
}
#ifdef _DEBUG
HRESULT CNonPlayer::RenderPart(){
	for(_uint i = 0; i < PART_END; ++i){
		if(nullptr == m_pPartColliderCom[i])
			continue;

		m_pPartColliderCom[i]->RenderCollider();

	}
	return NOERROR;
}
#endif
HRESULT CNonPlayer::SettingShader(){
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 각 변환행렬 변수들
	_matrix matWorld, matView, matProj;

	// 월드 행렬은 1인칭인 경우 카메라의 월드행렬을 사용
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

void CNonPlayer::CheckState(){
	switch(m_pDynamicMeshCom->GetCurrentAnaimationSet()){
	case CNonPlayer::ANIMATION_SET::ZOMBIE_IDLE:
		break;
	case CNonPlayer::ANIMATION_SET::ZOMBIE_ATTACK:
		if(m_pDynamicMeshCom->IsFinished()){
			m_bAttackCheck = false;
			m_pDynamicMeshCom->SetUpAnimationSet(m_eCurrAnimation = ANIMATION_SET::ZOMBIE_IDLE);
		}
		break;
	case CNonPlayer::ANIMATION_SET::ZOMBIE_WALK:
		break;
	case CNonPlayer::ANIMATION_SET::ZOMBIE_DEATH:
		if(m_pDynamicMeshCom->IsFinished())
			m_bIsDead = true;
		break;
	default:
		break;
	}
}

void CNonPlayer::SetDeath(){
	m_pDynamicMeshCom->SetUpAnimationSet(m_eCurrAnimation = CNonPlayer::ANIMATION_SET::ZOMBIE_DEATH);
	m_pPickeeCom->RemovePickeeDesc();


	if(rand() % 10 < 3){
		CManagement* pManagement = CManagement::GetInstance();
		if(nullptr == pManagement)
			return;
		SafeAddRef(pManagement);
		const _tchar* szWeaponName[6] = { L"AK47_Proto" , L"G3_Proto", L"RPD_Proto" ,L"M16M203_Proto" };

		_matrix matWorld;
		D3DXMatrixTranslation(&matWorld, m_PartWorldMatrx[HEAD].m[3][0], m_PartWorldMatrx[HEAD].m[3][1], m_PartWorldMatrx[HEAD].m[3][2]);

		CWeaponObject::WEAPONOBJECT_DESC tDesc = {};
		tDesc.pWeapon = dynamic_cast<CWeapon*>(pManagement->AddCloneComponent(szWeaponName[rand() % 4], STAGE_SCENE));
		tDesc.pWorldMatrix = &matWorld;

		if(FAILED(pManagement->AddCloneObjectToLayer(L"WeaponObject_Proto", STAGE_SCENE, L"WeaponLayer", &tDesc))){
			SafeRelease(tDesc.pWeapon);
			SafeRelease(pManagement);
			return;
		}

		SafeRelease(pManagement);
	}

	return;
}

void CNonPlayer::ChangeAnimation(ANIMATION_SET _eAniSet){
	if(nullptr == m_pDynamicMeshCom)
		return;

	_vec3 vPos = m_pPartColliderCom[PARENT]->GetPosition();

	if(0.5f < D3DXVec3Length(&(vPos - m_pTransformCom->GetState(CTransform::STATE_POSITION)))){
		m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
	}

	m_eCurrAnimation = _eAniSet;
	m_pDynamicMeshCom->SetUpAnimationSet(_eAniSet);

}

HRESULT CNonPlayer::CharacterMove(_double _dDeltaTime){
	if(m_eCurrAnimation == ZOMBIE_ATTACK && m_pDynamicMeshCom->IsFinished())
		return NOERROR;

	m_pDynamicMeshCom->SetUpAnimationSet(ANIMATION_SET::ZOMBIE_WALK);
	m_eCurrAnimation = ZOMBIE_WALK;

	CUnitManager* pUnitManager = CUnitManager::GetInstance();
	if(nullptr == pUnitManager)
		return E_FAIL;
	SafeAddRef(pUnitManager);

	// 플레이어 바라보게 만들기
	_vec3 vPlayerPos = pUnitManager->GetPlayerPos();
	_vec3 vPositon = m_pTransformCom->GetState(CTransform::STATE_POSITION);

	_vec3 vDir = vPlayerPos - vPositon;
	_float vLength = D3DXVec3Length(&vDir);
	D3DXVec3Normalize(&vDir, &vDir);

	vDir.y = 0;
	_vec3 vRight, vUp, vLook;

	D3DXVec3Normalize(&vLook, &vDir);
	D3DXVec3Normalize(&vRight, D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook));
	D3DXVec3Normalize(&vUp, D3DXVec3Cross(&vUp, &vLook, &vRight));

	m_pTransformCom->SetState(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->SetState(CTransform::STATE_UP, vUp);
	m_pTransformCom->SetState(CTransform::STATE_LOOK, vLook);

	m_pTransformCom->Move(_dDeltaTime, vDir, m_pNavigationCom);

	SafeRelease(pUnitManager);

	return NOERROR;
}

HRESULT CNonPlayer::Attack(){
	if(m_eCurrAnimation != ANIMATION_SET::ZOMBIE_ATTACK)
		m_pDynamicMeshCom->SetUpAnimationSet(m_eCurrAnimation = ANIMATION_SET::ZOMBIE_ATTACK);

	CGameObject* pGameObject = nullptr;

	if(!m_bAttackCheck)
		if(m_pPartColliderCom[RIGHT_ELBOW]->IsCollision() && (pGameObject = m_pPartColliderCom[RIGHT_ELBOW]->GetCollisionObject())){
			CUnitManager* pUnitManager = CUnitManager::GetInstance();
			if(nullptr == pUnitManager)
				return E_FAIL;
			SafeAddRef(pUnitManager);

			pUnitManager->SetPlayerDamage(c_fZombieAttackDamage);
			m_bAttackCheck = true;
			SafeRelease(pUnitManager);
		}

	return NOERROR;
}

// 원본 생성 함수(static)
CNonPlayer * CNonPlayer::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CStaticMeshObject* _pStaticMesh/* = nullptr*/){
	CNonPlayer* pInstance = new CNonPlayer(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pStaticMesh))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNonPlayer Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject * CNonPlayer::CreateClone(void * _pArgument){
	CNonPlayer* pInstance = new CNonPlayer(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNonPlayer Clone Failure");
	}

	return pInstance;
}

// Free
void CNonPlayer::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pFrustumCom);
	SafeRelease(m_pStaticMeshMap);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pDynamicMeshCom);
	SafeRelease(m_pNavigationCom);
	SafeRelease(m_pPickeeCom);
	for(_uint i = 0; i < PART_END; ++i){
		SafeRelease(m_pPartColliderCom[i]);
	}

	CGameObject::Free();
}
