#include "stdafx.h"
#include "Management.h"

#include "..\Header\Player.h"
#include "..\Header\StaticMeshObject.h"
#include "..\Header\Weapon.h"
#include "..\Header\NonPlayer.h"
#include "..\Header\WeaponObject.h"

#include "..\Header\UnitManager.h"

USING(Client)

// 생성자
CPlayer::CPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CCamera(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pMeleepColliderCom(nullptr)
	, m_pWeaponObjectColliderCom(nullptr)
	, m_pNavigationCom(nullptr)
	, m_pPickerCom(nullptr)
	, m_fGravityAcc(0.f)
	, m_dJumpTime(0.f)
	, m_fPlayerHP(c_fPlayerFullHP)
	, m_dPlayerHealingTimeAcc(0.0)
	, m_bIsDead(false){
	::ZeroMemory(m_pWeapon, sizeof(CWeapon*) * 2);
	::ZeroMemory(&m_vPrePos, sizeof(_vec3));
	::ZeroMemory(&m_tWeaponObjectColliderMatrix, sizeof(_matrix));
	::D3DXMatrixIdentity(&m_matShakeCamera);
}

// 복사 생성자
CPlayer::CPlayer(const CPlayer& _Player)
	: CCamera(_Player)
	, m_pRendererCom(_Player.m_pRendererCom)
	, m_pNavigationCom(_Player.m_pNavigationCom)
	, m_fGravityAcc(0.f)
	, m_dJumpTime(0.f)
	, m_vPrePos(_Player.m_vPrePos)
	, m_fPlayerHP(c_fPlayerFullHP)
	, m_dPlayerHealingTimeAcc(0.0)
	, m_bIsDead(false){
	::ZeroMemory(m_pWeapon, sizeof(CWeapon*) * 2);
	SafeAddRef(m_pRendererCom);
	SafeAddRef(m_pNavigationCom);
	::D3DXMatrixIdentity(&m_matShakeCamera);
}

// Initialize with Prototype
HRESULT CPlayer::InitGameObject_Proto(){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	return NOERROR;
}

// Initialize with Clone
HRESULT CPlayer::InitGameObject_Clone(void* _pArgument){
	PLAYER_DESC* pDesc = reinterpret_cast<PLAYER_DESC*>(_pArgument);
	if(FAILED(CCamera::InitGameObject_Clone(&(pDesc->tCameraDesc))))
		return E_FAIL;

	CUnitManager::GetInstance()->SetPlayerPos(reinterpret_cast<_vec3*>(m_pTransformCom->GetWorldMatrixPointer()->m[3]));
	CUnitManager::GetInstance()->SetPlayerHP(&m_fPlayerHP);

	// 시작시 설정되는 무기를 세팅함
	if(FAILED(WeaponSetting()))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.pGameObj = this;
	CollDesc.vScale = _vec3(1.2f, 1.2f, 1.2f);
	CollDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	CollDesc.pParentWorldMatrix = m_pTransformCom->GetWorldMatrixPointer();

	// 근접공격 콜라이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pMeleepColliderCom), L"MeleeColliderCom", L"OBB_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc)))
		return E_FAIL;

	// 콜라이더를 충돌매니저에 등록
	if(FAILED(m_pMeleepColliderCom->SettingToCollisionManager()))
		return E_FAIL;

	m_tWeaponObjectColliderMatrix = m_pTransformCom->GetWorldMatrix();
	m_tWeaponObjectColliderMatrix.m[3][1] -= c_fPlayerHeight;
	CollDesc.vScale = _vec3(1.f, 1.f, 1.f);
	CollDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	CollDesc.pParentWorldMatrix = &m_tWeaponObjectColliderMatrix;

	// 무기 오브젝트 콜라이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pWeaponObjectColliderCom), L"WeaponObjectColliderCom", L"Sphere_Collider_Proto", SCENE_TYPE::STATIC_SCENE, &CollDesc)))
		return E_FAIL;

	if(FAILED(m_pWeaponObjectColliderCom->SettingToCollisionManager()))
		return E_FAIL;


	// 네비게이션 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pNavigationCom), L"NavigationCom", L"Navigation_Proto", SCENE_TYPE::STAGE_SCENE, &(m_pTransformCom->GetState(CTransform::STATE_POSITION)))))
		return E_FAIL;
	
	// 픽커 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pPickerCom), L"PickerCom", L"PickerCom_Proto", SCENE_TYPE::STATIC_SCENE)))
		return E_FAIL;

	return NOERROR;
}

// LateInitialize
HRESULT CPlayer::LateInitGameObject(){
	return NOERROR;
}

// Update
HRESULT CPlayer::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	if(m_bIsDead)
		return NOERROR;

	PlayerControl(_dDeltaTime);

	if(FLAG_JUMP_START & m_bytePlayerFlag
		|| FLAG_JUMP_END & m_bytePlayerFlag)
		Jump(_dDeltaTime);
	
	if(!(FLAG_JUMP_START & m_bytePlayerFlag))
		LandingGround(_dDeltaTime);

	// 파이프라인에 뷰 변환 행렬과 투영 행렬 전달
	_matrix matWorld = m_matShakeCamera * m_pTransformCom->GetWorldMatrix();
	_matrix matView;
	D3DXMatrixInverse(&matView, nullptr, &matWorld);
	m_pPipeline->SetTransform(D3DTS_VIEW, matView);
	if(m_bytePlayerFlag & FLAG_ADS){
		_matrix adsProjMatrix;
		::D3DXMatrixPerspectiveFovLH(&adsProjMatrix, m_tCameraDesc.fFovY * 0.9f, m_tCameraDesc.fAspect, m_tCameraDesc.fNear, m_tCameraDesc.fFar);
		m_pPipeline->SetTransform(D3DTS_PROJECTION, adsProjMatrix);

	} else{
		m_pPipeline->SetTransform(D3DTS_PROJECTION, m_tProjectionMatrix);
	}
	m_pPipeline->SetCameraWorldMatrix(matWorld);

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	if(FLAG_WEAPON_CHANGE & m_bytePlayerFlag){
		m_eWeaponType = m_eWeaponType == PRIMARY_WEAPON ? SECONDARY_WEAPON : PRIMARY_WEAPON;
		m_bytePlayerFlag &= ~FLAG_WEAPON_CHANGE;
		CUnitManager::GetInstance()->SetPlayerWeapon(m_pWeapon[m_eWeaponType]);
	}

	if(nullptr == m_pWeapon[m_eWeaponType])
		return E_FAIL;

	m_pWeapon[m_eWeaponType]->UpdateWeapon(m_pPickerCom, _dDeltaTime);

	SafeRelease(pManagement);

	m_tWeaponObjectColliderMatrix = m_pTransformCom->GetWorldMatrix();
	m_tWeaponObjectColliderMatrix.m[3][1] -= c_fPlayerHeight;

	if(nullptr != m_pMeleepColliderCom)
		m_pMeleepColliderCom->UpdateCollider();

	if(nullptr != m_pWeaponObjectColliderCom)
		m_pWeaponObjectColliderCom->UpdateCollider();

	return NOERROR;
}

// LateUpdate
HRESULT CPlayer::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pWeapon[m_eWeaponType])
		return E_FAIL;

	if(m_bIsDead)
		return NOERROR;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	m_pRendererCom->AddRenderList(CRenderer::RENDER_PRIORITY, this);

	if(nullptr != m_pMeleepColliderCom)
		m_pMeleepColliderCom->IsCollision();

	// 떨어진 무기와 변경
	if(nullptr != m_pWeaponObjectColliderCom)
		if(m_pWeaponObjectColliderCom->IsCollision()){
			CWeaponObject* pWeaponObject = dynamic_cast<CWeaponObject*>(m_pWeaponObjectColliderCom->GetCollisionObject());
			if(nullptr != pWeaponObject){
				CUnitManager* pUnitManager = CUnitManager::GetInstance();
				if(nullptr != pUnitManager){
					SafeAddRef(pUnitManager);
					pUnitManager->SetCollisionGroundWeapon(true);
					pUnitManager->SetGroundWeaponName(pWeaponObject->GetName());
					if(pManagement->IsKeyDown(DIK_F)){
						pWeaponObject->ChangeWeapon(&m_pWeapon[m_eWeaponType]);
						m_pWeapon[m_eWeaponType]->SetPlayerFlag(&m_bytePlayerFlag);
						m_pWeapon[m_eWeaponType]->SetPlayerPicker(m_pPickerCom);
						m_pWeapon[m_eWeaponType]->ResetAnimationSound();

						CUnitManager::GetInstance()->SetPlayerWeapon(m_pWeapon[m_eWeaponType]);
					}

					SafeRelease(pUnitManager);
				}
			}
		} else
			CUnitManager::GetInstance()->SetCollisionGroundWeapon(false);

	_float fDamage = 0.f;
	if(fDamage = CUnitManager::GetInstance()->GetPlayerDamage()
		|| m_bIsCameraShaking){
		m_fPlayerHP -= fDamage;
		HitCamera(_dDeltaTime, 1.f);
		if(m_fPlayerHP <=  0){
			SetDead();
		}
	}
	SafeRelease(pManagement);

	return NOERROR;
}

// Render
HRESULT CPlayer::RenderGameObject(_double _dDeltaTime){
	if(nullptr != m_pWeapon[m_eWeaponType])
		m_pWeapon[m_eWeaponType]->RenderWeapon();

	return NOERROR;
}

// 시작시 설정되는 무기를 세팅
HRESULT CPlayer::WeaponSetting(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pWeapon[PRIMARY_WEAPON]), L"PrimaryWeapon_Com", L"AK47_Proto", STAGE_SCENE);
	CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pWeapon[SECONDARY_WEAPON]), L"SecondaryWeapon_Com", L"G3_Proto", STAGE_SCENE);

	if(nullptr == m_pWeapon[PRIMARY_WEAPON]
		&& nullptr == m_pWeapon[SECONDARY_WEAPON]){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_pWeapon[PRIMARY_WEAPON]->SetThirdPerson(false);
	m_pWeapon[PRIMARY_WEAPON]->SetPlayerFlag(&m_bytePlayerFlag);
	m_pWeapon[PRIMARY_WEAPON]->SetPlayerPicker(m_pPickerCom);

	m_pWeapon[SECONDARY_WEAPON]->SetThirdPerson(false);
	m_pWeapon[SECONDARY_WEAPON]->SetPlayerFlag(&m_bytePlayerFlag);
	m_pWeapon[SECONDARY_WEAPON]->SetPlayerPicker(m_pPickerCom);

	m_eWeaponType = PRIMARY_WEAPON;

	CUnitManager::GetInstance()->SetPlayerWeapon(m_pWeapon[m_eWeaponType]);

	SafeRelease(pManagement);

	return NOERROR;
}

// 플레이어 조작
void CPlayer::PlayerControl(_double _dDeltaTime){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return;
	SafeAddRef(pManagement);

	// 걷는 방향 계산(플레이어의 Right벡터와 월드의 Up벡터를 외적함)
	_vec3 vRight = m_pTransformCom->GetState(CTransform::STATE_RIGHT);
	_vec3 vDir = {};
	D3DXVec3Cross(&vDir, &vRight, &_vec3(0.f, 1.f, 0.f));

	// ADS 상태일때 총기류마다 속도 다르게
	_float fSpeed = 1.f;

	if(FLAG_ADS & m_bytePlayerFlag){
		switch(m_pWeapon[m_eWeaponType]->GetWeaponType()){
		case CWeapon::AR:
			fSpeed = 0.5f;
			break;
		case CWeapon::SMG:
		case CWeapon::SG:
			fSpeed = 0.8f;
			break;
		case CWeapon::PS:
			fSpeed = 1.f;
			break;
		case CWeapon::SR:
		case CWeapon::LMG:
			fSpeed = 0.3f;
			break;
		default:
			fSpeed = 1.f;
			break;
		}
	}

	// 스프린트 중이라면 이동속도 2배
	_float fSprintSpeed = m_bytePlayerFlag & PLAYER_FALG::FLAG_SPRINT ? 2.f : 1.f;
	
	if(!(FLAG_JUMP_START & m_bytePlayerFlag)
		&& !(FLAG_JUMP_END & m_bytePlayerFlag)){
		_vec3 vMoveResult = m_vPrePos = m_pTransformCom->GetState(CTransform::STATE_POSITION);

		// 키 입력
		if(pManagement->IsKeyPressing(DIK_W)){
			m_pTransformCom->Move(_dDeltaTime * fSprintSpeed * fSpeed, vDir, m_pNavigationCom);
			m_bIsWalk = true;
		}
		if(pManagement->IsKeyPressing(DIK_S)){
			m_pTransformCom->Move(-_dDeltaTime * fSpeed * 0.7f, vDir, m_pNavigationCom);
			m_bIsWalk = true;
		}
		if(pManagement->IsKeyPressing(DIK_A)){
			m_pTransformCom->Move(-_dDeltaTime * fSpeed * 0.8f, vRight, m_pNavigationCom);
			m_bIsWalk = true;
		}
		if(pManagement->IsKeyPressing(DIK_D)){
			m_pTransformCom->Move(_dDeltaTime * fSpeed * 0.8f, vRight, m_pNavigationCom);
			m_bIsWalk = true;
		}
		m_dWalkTime += _dDeltaTime;

		_double dSoundInterval = 0.0;
		
		if(m_bytePlayerFlag&FLAG_SPRINT)
			dSoundInterval = 0.3;
		else if(m_bytePlayerFlag&FLAG_ADS)
			dSoundInterval = 0.6;
		else
			dSoundInterval = 0.4;

		if(m_bIsWalk && m_dWalkTime > dSoundInterval){
			_tchar szWalkSoundkey[32];
			_tchar szNum[4];
			_itow_s((rand() % 4) + 1, szNum, 10);

			lstrcpy(szWalkSoundkey, L"foot_dirt0");
			lstrcat(szWalkSoundkey, szNum);
			lstrcat(szWalkSoundkey, L".wav");
			
			pManagement->PlaySound(szWalkSoundkey,1);
			m_dWalkTime = 0.0;
		}

		m_bIsWalk = false;
		
		m_vPrePos = m_pTransformCom->GetState(CTransform::STATE_POSITION) - m_vPrePos;
	}

	if(pManagement->IsKeyDown(DIK_SPACE)
		&& !(FLAG_JUMP_START & m_bytePlayerFlag)
		&& !(FLAG_JUMP_END & m_bytePlayerFlag)){
		m_bytePlayerFlag |= FLAG_JUMP_START;
		m_bytePlayerFlag &= ~FLAG_SPRINT;
	}

	// 마우스 움직임
	_long lMouseMove = 0;

	if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_X)){
		_vec3 vWorldUp(0.f, 1.f, 0.f);
		m_pTransformCom->RotationAxis(&vWorldUp, _dDeltaTime * lMouseMove * 0.1);
	}

	if(lMouseMove = pManagement->GetMouseMovement(CInputDevice::MOUSE_Y)){
		m_pTransformCom->RotationRightAxis(_dDeltaTime * lMouseMove * 0.1);
	}

	SafeRelease(pManagement);
}


// 점프
void CPlayer::Jump(_double _dDeltaTime){
	CUnitManager* pUnitManager = CUnitManager::GetInstance();
	if(nullptr == pUnitManager)
		return;
	SafeAddRef(pUnitManager);

	_vec3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vec3 vPrePos = m_vPrePos;
	vPrePos.y = 0.f;

	if((m_bytePlayerFlag & FLAG_JUMP_START)){
		m_dJumpTime += _dDeltaTime;
		_float JumpPower = 3.5f - GRAVITY_ACCEL(m_dJumpTime);

		_float fDist = 0.f;
		_vec3 vRayDir = { 0.f, 1.f, 0.f };

		if(pUnitManager->IntersectRayToStageMap(vRayDir, vPos, &fDist)){
			if(D3DXVec3Length(&(vRayDir * fDist)) < 0.3f)
				JumpPower = 0.f;
		}

		vPos += vPrePos + _vec3(0.f, JumpPower * static_cast<_float>(_dDeltaTime), 0.f);

		if(JumpPower <= 0.f){
		 	m_bytePlayerFlag &= ~FLAG_JUMP_START;
			m_bytePlayerFlag |= FLAG_JUMP_END;
			m_dJumpTime = 0.f;
		}
	}else
		vPos += vPrePos;

	if(m_pNavigationCom->MovingCheck(vPos))
		m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
	else{
		vPos.x -= vPrePos.x;
		vPos.z -= vPrePos.z;

		m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
	}

	SafeRelease(pUnitManager);
}

// 플레이어에 대한 자유낙하 계산
void CPlayer::LandingGround(_double _dDeltaTime){
	CUnitManager* pUnitManager = CUnitManager::GetInstance();
	if(nullptr == pUnitManager)
		return;
	SafeAddRef(pUnitManager);

	_vec3 vRayPos, vPos;
	vRayPos = vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	_vec3 vRayDir = { 0.f, -1.f, 0.f };
	_float fDist = 0.f;

	if(pUnitManager->IntersectRayToStageMap(vRayDir, vPos, &fDist)){
		vRayPos += (vRayDir * fDist);

		if(vRayPos.y + c_fPlayerHeight + GRAVITY_ACCEL(_dDeltaTime) < vPos.y){
			m_fGravityAcc += GRAVITY_ACCEL(_dDeltaTime);
			vPos.y -= m_fGravityAcc;
			if(vPos.y < vRayPos.y + c_fPlayerHeight){
				m_bytePlayerFlag &= ~FLAG_JUMP_END;
				vPos.y = vRayPos.y + c_fPlayerHeight;
				m_fGravityAcc = 0.f;
			}
		} else{
			m_bytePlayerFlag &= ~FLAG_JUMP_END;
			vPos.y = vRayPos.y + c_fPlayerHeight;
			m_fGravityAcc = 0.f;
		}
	}

	SafeRelease(pUnitManager);

	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);
}

void CPlayer::HitCamera(_double _dDeltaTime, _float _fPower){
	if(!m_bIsCameraShaking){
		m_fPower = _fPower * (rand() & 1 ? 1.f : -1.f);
		m_bIsCameraShakingEnd = false;
		m_bIsCameraShaking = true;
		D3DXMatrixIdentity(&m_matShakeCamera);
	}
	_matrix matRotateX, matRotateY;

	if(!m_bIsCameraShakingEnd){
		D3DXMatrixRotationX(&matRotateX, D3DXToRadian(-_fPower));
		D3DXMatrixRotationY(&matRotateY, D3DXToRadian(m_fPower));
		m_matShakeCamera *= matRotateX* matRotateY;
		if(m_dCameraShakingTimeAcc > 0.075){
			m_bIsCameraShakingEnd = true;
		}
	} else{
		D3DXMatrixRotationX(&matRotateX, D3DXToRadian(_fPower));
		D3DXMatrixRotationY(&matRotateY, D3DXToRadian(m_fPower));
		m_matShakeCamera *= matRotateX* matRotateY;
		if(m_dCameraShakingTimeAcc > 0.15){
			m_bIsCameraShaking = false;
			m_dCameraShakingTimeAcc = 0.0;
			D3DXMatrixIdentity(&m_matShakeCamera);
		}
	}

	m_dCameraShakingTimeAcc += _dDeltaTime;
}

void CPlayer::SetDead(){
	m_bIsDead = true;

	_vec3 vPos = m_pTransformCom->GetState(CTransform::STATE_POSITION);
	
	vPos.y -= c_fPlayerHeight - 0.3f;
	m_pTransformCom->SetState(CTransform::STATE_POSITION, vPos);

	_vec3 vLook = m_pTransformCom->GetState(CTransform::STATE_LOOK);

	m_pTransformCom->RotationAxis(&vLook, -0.5f);

	_matrix matView;
	D3DXMatrixInverse(&matView, nullptr, &m_pTransformCom->GetWorldMatrix());

	m_pPipeline->SetTransform(D3DTS_VIEW, matView);
}

// 원본 생성 함수(static)
CPlayer * CPlayer::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CPlayer* pInstance = new CPlayer(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CPlayer Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject * CPlayer::CreateClone(void* _pArgument){
	CGameObject* pInstance = new CPlayer(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CPlayer Clone Failure");
	}

	return pInstance;
}

// Free
void CPlayer::Free(){
	SafeRelease(m_pWeapon[PRIMARY_WEAPON]);
	SafeRelease(m_pWeapon[SECONDARY_WEAPON]);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pMeleepColliderCom);
	SafeRelease(m_pWeaponObjectColliderCom);
	SafeRelease(m_pNavigationCom);
	SafeRelease(m_pPickerCom);

	CCamera::Free();
}