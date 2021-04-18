#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\M4M203.h"
#include "..\Header\M4M203_Grenade.h"
#include "Timer.h"

USING(Client)

Cm4m203::Cm4m203(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice)
	, m_pM203(nullptr){
}

Cm4m203::Cm4m203(const Cm4m203 & _m4m203)
	: CWeapon(_m4m203)
	, m_pM203(nullptr){
}

// Initialize with Prototype
HRESULT Cm4m203::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_m4m203_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}


	// 3인칭 모델 컴포넌트 추가
	m_pThirdPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"weapon_m4m203_Proto", STAGE_SCENE));
	if(nullptr == m_pThirdPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 총기 세부 설정
	m_eWeaponType = CWeapon::AR;				// 무기타입
	m_iMaxOneMagazine = 30;						// 한 탄창의 탄약 수
	m_iCurrentMagazine = 30;					// 현재 탄창의 탄약 수
	m_iMaxAmmo = 450;							// 최대 소지 탄약 수
	m_iCurrentAmmo = 120;						// 현재 소지 탄약 수
	m_fFiringRate = 1.f / 14.283f;

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT Cm4m203::InitComponent_Clone(void * _pArgument){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	m_pM203 = dynamic_cast<CM4M203_Grenade*>(pManagement->AddCloneComponent(L"M4M203_Grenade_Proto", STAGE_SCENE));
	if(nullptr == m_pM203)
		return E_FAIL;

	dynamic_cast<CM4M203_Grenade*>(m_pM203)->SetBulletBool(&m_bIsGrenadeLauncher);

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT Cm4m203::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);

	m_pM203->SetThirdPerson(false);
	m_pM203->SetPlayerFlag(m_pPlayerFlag);

	return NOERROR;
}

HRESULT Cm4m203::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
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

HRESULT Cm4m203::RenderWeapon(){
	if(!m_bIsGrenadeLauncher)
		CWeapon::RenderWeapon();
	else
		m_pM203->RenderWeapon();

	return NOERROR;
}

HRESULT Cm4m203::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 발사
	m_bIsFire = false;
	if(eCurrentSet == IDLE || eCurrentSet == FIRE){
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
			m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
			POINT pt = { WINCX >> 1, WINCY >> 1 };
			if(m_dAccFiringTime >= m_fFiringRate
				&& !FAILED(Fire(pt, _pPicker))){
				m_pFirstPersonModelCom->ResetTime();
				m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);

				m_dAccFiringTime = 0.0;
			}
		}
	} else if(ADS_IDLE == eCurrentSet || ADS_FIRE == eCurrentSet){
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON)){
			m_dAccFiringTime += m_pFiringTimer->ComputeDeltaTime();
			POINT pt = { WINCX >> 1, WINCY >> 1 };
			if(m_dAccFiringTime >= m_fFiringRate
				&& !FAILED(Fire(pt, _pPicker))){
				m_pFirstPersonModelCom->ResetTime();
				m_pFirstPersonModelCom->SetUpAnimationSet(ADS_FIRE, 1.5f);

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

	// 유탄발사기로 교체
	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
		if(pManagement->IsKeyDown(DIK_5)){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_bIsGrenadeLauncher = true;
		}
	}
	// 나이트비전 사용

	// 나이트비전 사용 종료


	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT Cm4m203::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	switch(eCurrentSet){
	case Cm4m203::IDLE:
		return NOERROR;
		break;
	case Cm4m203::FIRE:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case Cm4m203::ADS_IDLE:
		return NOERROR;
		break;
	case Cm4m203::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case Cm4m203::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::RELOAD:
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm4m203::RELOAD_EMPTY:
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case Cm4m203::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		break;
	case Cm4m203::SPRINT_LOOP:
		break;
	case Cm4m203::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::FIRST_TIME_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			return NOERROR;
		}
		break;
	case Cm4m203::QUICK_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::QUICK_PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case Cm4m203::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case Cm4m203::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case Cm4m203::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case Cm4m203::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

Cm4m203 * Cm4m203::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	Cm4m203* pInstance = new Cm4m203(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"Cm4m203 Create Failure");
	}

	return pInstance;
}

CComponent * Cm4m203::CreateClone(void * _pArgument){
	CComponent* pInstance = new Cm4m203(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"Cm4m203 Create Failure");
	}

	return pInstance;
}

void Cm4m203::Free(){
	SafeRelease(m_pM203);

	CWeapon::Free();
}
