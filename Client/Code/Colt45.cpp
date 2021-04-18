#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Colt45.h"

USING(Client)

CColt45::CColt45(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
}

CColt45::CColt45(const CColt45 & _Beretta)
	: CWeapon(_Beretta){
}

HRESULT CColt45::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_colt45_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가
	/*if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pThirdPersonModelCom), L"weapon_Beretta", L"weapon_Beretta_Proto", SCENE_TYPE::STAGE_SCENE)))
	return E_FAIL;*/

	// 총기 세부 설정
	m_eWeaponType = CWeapon::PS;				// 무기타입
	m_iMaxOneMagazine = 7;						// 한 탄창의 탄약 수
	m_iCurrentMagazine = 7;						// 현재 탄창의 탄약 수
	m_iMaxAmmo = 28;							// 최대 소지 탄약 수
	m_iCurrentAmmo = 14;						// 현재 소지 탄약 수

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

HRESULT CColt45::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT CColt45::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);

	return NOERROR;
}

HRESULT CColt45::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	if(!m_bIsThirdPerson){
		if(!m_bIsLateInit){
			CWeapon::LateInitWeapon();
		}

		WeaponControl(_pPicker);
		UpdateWeaponState(_dDeltaTime);

	}
	return CWeapon::UpdateWeapon(_pPicker, _dDeltaTime);
}

HRESULT CColt45::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

HRESULT CColt45::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 사격
	if(eCurrentSet == IDLE || eCurrentSet == FIRE){
		if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_LBUTTON)){
			if(0 != m_iCurrentMagazine){
				if(m_iCurrentMagazine == 1)
					m_pFirstPersonModelCom->SetUpAnimationSet(LASTFIRE, 1.5f);
				else
					m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);

			}
		}
	} else if(eCurrentSet == ADS_IDLE || eCurrentSet == ADS_FIRE){
		if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_LBUTTON)){
			if(0 != m_iCurrentMagazine){
				if(m_iCurrentMagazine == 1)
					m_pFirstPersonModelCom->SetUpAnimationSet(ADS_LASTFIRE, 1.5f);
				else
					m_pFirstPersonModelCom->SetUpAnimationSet(ADS_FIRE, 1.5f);
			}
		}
	}

	// 재장전
	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
		if(pManagement->IsKeyDown(DIK_R)
			/*			&& 0 != m_iCurrentAmmo
			&& m_iCurrentMagazine != m_iMaxOneMagazine*/){
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
		if(nullptr != m_pPlayerFlag)
			*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
		if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_RBUTTON)){
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_DOWN, 1.5f);
		}
	}

	// 스프린트 시작
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

	// 스프린트 끝
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

HRESULT CColt45::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	switch(eCurrentSet){
	case CColt45::IDLE:
		return NOERROR;
		break;
	case CColt45::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			--m_iCurrentMagazine;
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CColt45::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case CColt45::ADS_IDLE:
		return NOERROR;
		break;
	case CColt45::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			--m_iCurrentMagazine;
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		}
		break;
	case CColt45::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::RELOAD:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::RELOAD_EMPTY:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		break;
	case CColt45::SPRINT_LOOP:
		break;
	case CColt45::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::FIRST_TIME_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			return NOERROR;
		}
		break;
	case CColt45::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CColt45::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CColt45::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CColt45::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

CColt45 * CColt45::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CColt45* pInstance = new CColt45(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CColt45 Create Failure");
	}

	return pInstance;
}

CComponent * CColt45::CreateClone(void * _pArgument){
	CComponent* pInstance = new CColt45(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CColt45 Clone Failure");
	}

	return pInstance;
}

void CColt45::Free(){
	CWeapon::Free();
}
