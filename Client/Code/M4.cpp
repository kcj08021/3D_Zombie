#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\M4.h"

USING(Client)

CM4::CM4(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
}

CM4::CM4(const CM4 & _m4m203)
	: CWeapon(_m4m203){
}

// Initialize with Prototype
HRESULT CM4::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_m4_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가
	m_pThirdPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"weapon_m4_Proto", STAGE_SCENE));
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
	m_fDamage = 25.f;

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT CM4::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT CM4::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);

	return NOERROR;
}

HRESULT CM4::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
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

HRESULT CM4::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

HRESULT CM4::WeaponControl(CPicker* _pPicker){
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

	// 나이트비전 사용

	// 나이트비전 사용 종료


	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CM4::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	switch(eCurrentSet){
	case CM4::IDLE:
		return NOERROR;
		break;
	case CM4::FIRE:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case CM4::ADS_IDLE:
		return NOERROR;
		break;
	case CM4::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case CM4::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::RELOAD:
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CM4::RELOAD_EMPTY:
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CM4::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		break;
	case CM4::SPRINT_LOOP:
		break;
	case CM4::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::FIRST_TIME_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			return NOERROR;
		}
		break;
	case CM4::QUICK_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::QUICK_PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM4::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case CM4::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case CM4::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CM4::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

CM4 * CM4::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CM4* pInstance = new CM4(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CM4 Create Failure");
	}

	return pInstance;
}

CComponent * CM4::CreateClone(void * _pArgument){
	CComponent* pInstance = new CM4(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CM4 Create Failure");
	}

	return pInstance;
}

void CM4::Free(){
	CWeapon::Free();
}
