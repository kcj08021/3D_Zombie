#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\AK47.h"

USING(Client)

// 생성자
Cak47::Cak47(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice)
	{
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

// 복사 생성자
Cak47::Cak47(const Cak47 & _ak47)
	: CWeapon(_ak47){
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

// Initialize with Prototype
HRESULT Cak47::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_ak47_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가
	m_pThirdPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"weapon_ak47_Proto", STAGE_SCENE));
	if(nullptr == m_pThirdPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	m_szWaeponName = new _tchar[5];
	lstrcpy(m_szWaeponName, L"AK47");
	// 총기 세부 설정
	m_eWeaponType = CWeapon::AR;				// 무기타입
	m_iMaxOneMagazine = c_iAK47_MaxOneMagazine;						// 한 탄창의 탄약 수
	m_iCurrentMagazine = c_iAK47_MaxOneMagazine;					// 현재 탄창의 탄약 수
	m_iMaxAmmo = c_iAK47_MaxAmmo;							// 최대 소지 탄약 수
	m_iCurrentAmmo = c_iAK47_InitAmmo;						// 현재 소지 탄약 수
	m_fFiringRate = c_fAK47_FiringRate;
	m_fDamage = c_fAK47_Damage;

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT Cak47::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

// LateInitialize
HRESULT Cak47::LateInitWeapon(){
	if(!m_bIsThirdPerson){
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);
		ResetAnimationSound();
	}

	return NOERROR;
}

// Update
HRESULT Cak47::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	if(!m_bIsThirdPerson){
		if(!m_bIsLateInit){
			CWeapon::LateInitWeapon();
		}

		CWeapon::UpdateWeapon(_pPicker, _dDeltaTime);

		WeaponControl(_pPicker);

		if(m_bIsGrenade)
			UpdateGrenadeState(_dDeltaTime);
		else
			UpdateWeaponState(_dDeltaTime);

	}

	return NOERROR;
}

// Render
HRESULT Cak47::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

// 조작
HRESULT Cak47::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);


	if(!m_bIsGrenade){// 사격
		if(eCurrentSet == IDLE || eCurrentSet == FIRE){
			if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
				m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
				POINT pt = { (WINCX >> 1) + (50 - (rand() % 101)), (WINCY >> 1) + (50 - (rand() % 101)) };
				if(m_dAccFiringTime >= m_fFiringRate
					&& !FAILED(Fire(pt, _pPicker))){
					m_pFirstPersonModelCom->ResetTime();
					m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);
					pManagement->PlaySound(L"weap_ak47_slst_3.wav", 1);

					m_dAccFiringTime = 0.0;
				}
			}
		} else if(ADS_IDLE == eCurrentSet || ADS_FIRE == eCurrentSet){
			if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
				m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
				POINT pt = { (WINCX >> 1), (WINCY >> 1) };
				if(m_dAccFiringTime >= m_fFiringRate
					&& !FAILED(Fire(pt, _pPicker))){
					m_pFirstPersonModelCom->ResetTime();
					m_pFirstPersonModelCom->SetUpAnimationSet(ADS_FIRE, 1.5f);
					pManagement->PlaySound(L"weap_ak47_slst_3.wav", 1);

					m_dAccFiringTime = 0.0;
				}
			}
		}

		// 재장전
		if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
			if(pManagement->IsKeyDown(DIK_R)
				&& 0 != m_iCurrentAmmo
				&& m_iCurrentMagazine != m_iMaxOneMagazine){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
				m_pFirstPersonModelCom->SetUpAnimationSet(RELOAD, 1.5f);
			} else if(!pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)
				&& 0 == m_iCurrentMagazine
				&& 0 != m_iCurrentAmmo){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
				m_pFirstPersonModelCom->SetUpAnimationSet(RELOAD_EMPTY, 1.5f);
			}
		}


		// 정조준
		if(eCurrentSet == IDLE){
			if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_RBUTTON)){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag |= CPlayer::FLAG_ADS;
				m_pFirstPersonModelCom->SetUpAnimationSet(ADS_UP, 1.5f);
			}
		} else if(eCurrentSet == ADS_IDLE){
			if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_RBUTTON)){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
				m_pFirstPersonModelCom->SetUpAnimationSet(ADS_DOWN, 1.5f);
			}
		}

		// 달리기
		if((eCurrentSet == IDLE || eCurrentSet == ADS_IDLE)
			&& (!(*m_pPlayerFlag & CPlayer::FLAG_JUMP_START) && !(*m_pPlayerFlag & CPlayer::FLAG_JUMP_END))){
			if(pManagement->IsKeyDown(DIK_LSHIFT) && pManagement->IsKeyPressing(DIK_W)){
				m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_IN, 1.5f);
				if(nullptr != m_pPlayerFlag){
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
					*m_pPlayerFlag |= CPlayer::FLAG_SPRINT;
				}
			}
		}

		// 달리기 종료
		if(eCurrentSet == SPRINT_LOOP){
			if(pManagement->IsKeyUp(DIK_W)
				|| !(*m_pPlayerFlag & CPlayer::FLAG_SPRINT)){
				m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_OUT, 1.5f);
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_SPRINT;
			}
		}

		// 근접공격
		if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
			if(pManagement->IsKeyUp(DIK_V)){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
				m_bIsMeleeAttack = true;
				m_pFirstPersonModelCom->SetUpAnimationSet(KNIFE_MELEE1, 1.5f);
			}
		}

		//무기 교체
		if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
			if(pManagement->IsKeyDown(DIK_1)
				|| pManagement->IsKeyDown(DIK_2)){
				if(nullptr != m_pPlayerFlag)
					*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
				m_pFirstPersonModelCom->SetUpAnimationSet(PUTAWAY, 1.5f);
			}
		}
	}

	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE || eCurrentSet == QUICK_PUTAWAY){
		if(Grenade()){
			*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			if(eCurrentSet != QUICK_PUTAWAY)
				m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PUTAWAY, 1.5f);
		} else if(m_bIsGrenade){
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		}
	}

	SafeRelease(pManagement);

	return NOERROR;
}

// 애니메이션 업데이트
HRESULT Cak47::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());
	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	switch(eCurrentSet){
	case Cak47::IDLE:
		SafeRelease(pManagement);
		return NOERROR;
		break;
	case Cak47::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::ADS_IDLE:
		SafeRelease(pManagement);
		return NOERROR;
		break;
	case Cak47::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::RELOAD:
		if(m_dAnimationTime > 0.5 && !m_bAnimationSound[SOUND_CLIP_OUT]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_clipout_v5.wav", 1);
			m_bAnimationSound[SOUND_CLIP_OUT] = true;
		}
		if(m_dAnimationTime > 1.6 && !m_bAnimationSound[SOUND_CLIP_IN]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_clipin_v4.wav", 1);
			m_bAnimationSound[SOUND_CLIP_IN] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::RELOAD_EMPTY:
		if(m_dAnimationTime > 0.5 && !m_bAnimationSound[SOUND_CLIP_OUT]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_clipout_v5.wav", 1);
			m_bAnimationSound[SOUND_CLIP_OUT] = true;
		}
		if(m_dAnimationTime > 1.6 && !m_bAnimationSound[SOUND_CLIP_IN]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_clipin_v4.wav", 1);
			m_bAnimationSound[SOUND_CLIP_IN] = true;
		}
		if(m_dAnimationTime > 2.3 && !m_bAnimationSound[SOUND_CHAMBER]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_chamber_v4.wav", 1);
			m_bAnimationSound[SOUND_CHAMBER] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::SPRINT_LOOP:
		break;
	case Cak47::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::FIRST_TIME_PULLOUT:
		if(m_dAnimationTime > 0.3 && !m_bAnimationSound[SOUND_CHAMBER]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_chamber_v4.wav", 1);
			m_bAnimationSound[SOUND_CHAMBER] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::PULLOUT:
		if(!m_bAnimationSound[SOUND_LIFT]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_lift_v4.wav", 1);
			m_bAnimationSound[SOUND_LIFT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();

			SafeRelease(pManagement);
			return NOERROR;
		}
		break;
	case Cak47::QUICK_PULLOUT:
		if(!m_bAnimationSound[SOUND_LIFT]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_lift_v4.wav", 1);
			m_bAnimationSound[SOUND_LIFT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::QUICK_PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
			m_bIsGrenade = true;
			ResetAnimationSound();
		}
		break;
	case Cak47::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_bIsMeleeAttack = false;
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_bIsMeleeAttack = false;
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cak47::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);
	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT Cak47::ResetAnimationSound(){
	m_dAnimationTime = 0;
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));

	return NOERROR;
}

Cak47 * Cak47::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	Cak47* pInstance = new Cak47(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"Cak47 Create Failure");
	}

	return pInstance;
}

CComponent * Cak47::CreateClone(void * pArgument){
	CComponent* pInstance = new Cak47(*this);
	if(FAILED(pInstance->InitComponent_Clone(pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"Cak47 Clone Failure");
	}
	return pInstance;
}

void Cak47::Free(){
	if(!CWeapon::IsClone())
		SafeDelete_Array(m_szWaeponName);

	CWeapon::Free();
}
