#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\RPD.h"


USING(Client)

// 생성자
CRPD::CRPD(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

// 복사 생성자
CRPD::CRPD(const CRPD & _ak47)
	: CWeapon(_ak47){
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

// Initialize with Prototype
HRESULT CRPD::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_rpd_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가
	m_pThirdPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"weapon_rpd_Proto", STAGE_SCENE));
	if(nullptr == m_pThirdPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}


	// 총기 세부 설정
	m_szWaeponName = new _tchar[4];
	lstrcpy(m_szWaeponName, L"RPD");

	m_eWeaponType = CWeapon::LMG;								// 무기타입
	m_iMaxOneMagazine = c_iRPD_MaxOneMagazine;					// 한 탄창의 탄약 수
	m_iCurrentMagazine = c_iRPD_MaxOneMagazine;					// 현재 탄창의 탄약 수
	m_iMaxAmmo = c_iRPD_MaxAmmo;								// 최대 소지 탄약 수
	m_iCurrentAmmo = c_iRPD_InitAmmo;							// 현재 소지 탄약 수
	m_fDamage = c_fRPD_Damage;
	m_fFiringRate = c_fRPD_FiringRate;

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT CRPD::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

// LateInitialize
HRESULT CRPD::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);

	return NOERROR;
}

// Update
HRESULT CRPD::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
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
HRESULT CRPD::RenderWeapon(){
	return CWeapon::RenderWeapon();
}

// 조작
HRESULT CRPD::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 사격
	m_bIsFire = false;
	if(eCurrentSet == IDLE || eCurrentSet == FIRE){
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
			m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
			POINT pt = { (WINCX >> 1) + (100 - (rand() % 201)), (WINCY >> 1) + (100 - (rand() % 201)) };
			if(m_dAccFiringTime >= m_fFiringRate
				&& !FAILED(Fire(pt, _pPicker))){
				m_pFirstPersonModelCom->ResetTime();
				m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);
				pManagement->PlaySound(L"weap_rpd_slmn_1.wav", 1);
				m_dAccFiringTime = 0.0;
			}
		}
	} else if(eCurrentSet == ADS_IDLE || eCurrentSet == ADS_FIRE){
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
			m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
			POINT pt = { (WINCX >> 1), (WINCY >> 1) };
			if(m_dAccFiringTime >= m_fFiringRate
				&& !FAILED(Fire(pt, _pPicker))){
				m_pFirstPersonModelCom->ResetTime();
				m_pFirstPersonModelCom->SetUpAnimationSet(ADS_FIRE, 1.5f);
				pManagement->PlaySound(L"weap_rpd_slmn_1.wav", 1);
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
			m_pFirstPersonModelCom->SetUpAnimationSet(RELOAD, 1.5f);
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

	SafeRelease(pManagement);

	return NOERROR;
}

// 애니메이션 업데이트
HRESULT CRPD::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	switch(eCurrentSet){
	case CRPD::IDLE:
		SafeRelease(pManagement);
		return NOERROR;
		break;
	case CRPD::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::ADS_IDLE:
		ResetAnimationSound();
		SafeRelease(pManagement);
		return NOERROR;
		break;
	case CRPD::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::RELOAD:
		if(m_dAnimationTime > 0.5 && !m_bAnimationSound[SOUND_CHAMBER]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_chamber_v1.wav", 1);
			m_bAnimationSound[SOUND_CHAMBER] = true;
		}
		if(m_dAnimationTime > 2.2 && !m_bAnimationSound[SOUND_OPEN]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_open_v1.wav", 1);
			m_bAnimationSound[SOUND_OPEN] = true;
		}
		if(m_dAnimationTime > 3.0 && !m_bAnimationSound[SOUND_CLIP_OUT]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_clipout_v1.wav", 1);
			m_bAnimationSound[SOUND_CLIP_OUT] = true;
		}
		if(m_dAnimationTime > 4.0 && !m_bAnimationSound[SOUND_CLIP_IN]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_clipin_v1.wav", 1);
			m_bAnimationSound[SOUND_CLIP_IN] = true;
		}
		if(m_dAnimationTime > 5.85 && !m_bAnimationSound[SOUND_CLOSE]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_close_v1.wav", 1);
			m_bAnimationSound[SOUND_CLOSE] = true;
		}
		if(m_dAnimationTime > 6.35 && !m_bAnimationSound[SOUND_HIT]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_hit_v1.wav", 1);
			m_bAnimationSound[SOUND_HIT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::SPRINT_LOOP:
		break;
	case CRPD::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::PULLOUT:
		if(!m_bAnimationSound[SOUND_LIFT]){
			pManagement->PlaySound(L"wpfoly_rpd_reload_lift_v1.wav", 1);
			m_bAnimationSound[SOUND_LIFT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::PUTAWAY:
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
	case CRPD::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	case CRPD::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			ResetAnimationSound();
		}
		break;
	default:
		break;
	}

	SafeRelease(pManagement);

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

HRESULT CRPD::ResetAnimationSound(){
	m_dAnimationTime = 0;
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));

	return NOERROR;
}

CRPD * CRPD::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CRPD* pInstance = new CRPD(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CRPD Create Failure");
	}

	return pInstance;
}

CComponent * CRPD::CreateClone(void * pArgument){
	CComponent* pInstance = new CRPD(*this);
	if(FAILED(pInstance->InitComponent_Clone(pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CRPD Clone Failure");
	}
	return pInstance;
}

void CRPD::Free(){
	if(!CWeapon::IsClone())
		SafeDelete_Array(m_szWaeponName);

	CWeapon::Free();
}
