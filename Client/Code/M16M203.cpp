#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\M16M203.h"

USING(Client)

Cm16m203::Cm16m203(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

Cm16m203::Cm16m203(const Cm16m203 & _m16m203)
	: CWeapon(_m16m203){
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));
}

// Initialize with Prototype
HRESULT Cm16m203::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_m16m203_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가
	m_pThirdPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"weapon_m16m203_Proto", STAGE_SCENE));
	if(nullptr == m_pThirdPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}


	// 총기 세부 설정
	m_szWaeponName = new _tchar[4];
	lstrcpy(m_szWaeponName, L"M16");
	m_eWeaponType = CWeapon::AR;							// 무기타입
	m_iMaxOneMagazine = c_iM16_MaxOneMagazine;				// 한 탄창의 탄약 수
	m_iCurrentMagazine = c_iM16_MaxOneMagazine;				// 현재 탄창의 탄약 수
	m_iMaxAmmo = c_iM16_MaxAmmo;							// 최대 소지 탄약 수
	m_iCurrentAmmo = c_iM16_InitAmmo;						// 현재 소지 탄약 수
	m_fFiringRate = c_fM16_FiringRate;
	m_fDamage = c_fM16_Damage;

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT Cm16m203::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT Cm16m203::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);

	return NOERROR;
}

HRESULT Cm16m203::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	if(!m_bIsThirdPerson){
		if(!m_bIsLateInit){
			CWeapon::LateInitWeapon();
		}

		CWeapon::UpdateWeapon(_pPicker, _dDeltaTime);

		WeaponControl(_pPicker);
		UpdateWeaponState(_dDeltaTime);

	}

	return NOERROR;
}

HRESULT Cm16m203::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

HRESULT Cm16m203::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	if(eCurrentSet == IDLE || eCurrentSet == FIRE){
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
			m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
			POINT pt = { (WINCX >> 1) + (50 - (rand() % 101)), (WINCY >> 1) + (50 - (rand() % 101)) };
			if(m_dAccFiringTime >= m_fFiringRate
				&& !FAILED(Fire(pt, _pPicker))){
				m_pFirstPersonModelCom->ResetTime();
				m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);
				pManagement->PlaySound(L"weap_m4_slst_1c4.wav", 1);
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
				pManagement->PlaySound(L"weap_m4_slst_1c4.wav", 1);
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

	// 근접 공격
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

	// 나이트비전 사용

	// 나이트비전 사용 종료


	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT Cm16m203::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	switch(eCurrentSet){
	case Cm16m203::IDLE:
		ResetAnimationSound();
		SafeRelease(pManagement);
		return NOERROR;
		break;
	case Cm16m203::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cm16m203::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished()){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
			ResetAnimationSound();
		}
		break;
	case Cm16m203::ADS_IDLE:
		SafeRelease(pManagement);
		ResetAnimationSound();
		return NOERROR;
		break;
	case Cm16m203::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		}
		break;
	case Cm16m203::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::RELOAD:
		if(m_dAnimationTime > 0.3 && !m_bAnimationSound[SOUND_CLIP_OUT]){
			pManagement->PlaySound(L"wpfoly_m4_reload_clipout.wav", 1);
			m_bAnimationSound[SOUND_CLIP_OUT] = true;
		}
		if(m_dAnimationTime > 0.9 && !m_bAnimationSound[SOUND_CLIP_IN]){
			pManagement->PlaySound(L"wpfoly_m4_reload_clipin.wav", 1);
			m_bAnimationSound[SOUND_CLIP_IN] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::RELOAD_EMPTY:
		if(m_dAnimationTime > 0.3 && !m_bAnimationSound[SOUND_CLIP_OUT]){
			pManagement->PlaySound(L"wpfoly_m4_reload_clipout.wav", 1);
			m_bAnimationSound[SOUND_CLIP_OUT] = true;
		}
		if(m_dAnimationTime > 0.9 && !m_bAnimationSound[SOUND_CLIP_IN]){
			pManagement->PlaySound(L"wpfoly_m4_reload_clipin.wav", 1);
			m_bAnimationSound[SOUND_CLIP_IN] = true;
		}
		if(m_dAnimationTime > 1.45 && !m_bAnimationSound[SOUND_CHAMBER2]){
			pManagement->PlaySound(L"wpfoly_m4_reload_chamber.wav", 1);
			m_bAnimationSound[SOUND_CHAMBER2] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		}
		break;
	case Cm16m203::SPRINT_LOOP:
		break;
	case Cm16m203::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::FIRST_TIME_PULLOUT:
		if(m_dAnimationTime > 0.3 && !m_bAnimationSound[SOUND_CHAMBER1]){
			pManagement->PlaySound(L"wpfoly_ak47_reload_chamber_v4.wav", 1);
			m_bAnimationSound[SOUND_CHAMBER1] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::PULLOUT:
		if(!m_bAnimationSound[SOUND_LIFT]){
			pManagement->PlaySound(L"wpfoly_m4_reload_lift.wav", 1);
			m_bAnimationSound[SOUND_LIFT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			SafeRelease(pManagement);
			return NOERROR;
		}
		break;
	case Cm16m203::QUICK_PULLOUT:
		if(!m_bAnimationSound[SOUND_LIFT]){
			pManagement->PlaySound(L"wpfoly_m4_reload_lift.wav", 1);
			m_bAnimationSound[SOUND_LIFT] = true;
		}
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::QUICK_PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm16m203::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		}
		break;
	case Cm16m203::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		}
		break;
	case Cm16m203::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		}
		break;
	case Cm16m203::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished()){
			ResetAnimationSound();
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		}
		break;
	default:
		break;
	}

	SafeRelease(pManagement);
	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

HRESULT Cm16m203::ResetAnimationSound(){
	m_dAnimationTime = 0;
	::ZeroMemory(m_bAnimationSound, sizeof(m_bAnimationSound));

	return NOERROR;
}

Cm16m203 * Cm16m203::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	Cm16m203* pInstance = new Cm16m203(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"Cm16m203 Create Failure");
	}

	return pInstance;
}

CComponent * Cm16m203::CreateClone(void * _pArgument){
	CComponent* pInstance = new Cm16m203(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"Cm16m203 Create Failure");
	}

	return pInstance;
}

void Cm16m203::Free(){
	if(!CWeapon::IsClone())
		SafeDelete_Array(m_szWaeponName);

	CWeapon::Free();
}
