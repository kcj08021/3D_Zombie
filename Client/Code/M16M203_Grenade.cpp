#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\M16M203_Grenade.h"

USING(Client)

CM16M203_Grenade::CM16M203_Grenade(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
}

CM16M203_Grenade::CM16M203_Grenade(const CM16M203_Grenade & _m4m203)
	: CWeapon(_m4m203){
}

// Initialize with Prototype
HRESULT CM16M203_Grenade::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1인칭 모델 컴포넌트 추가
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_m16m203_grenade_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3인칭 모델 컴포넌트 추가

	// 총기 세부 설정
	m_eWeaponType = CWeapon::GL;				// 무기타입
	m_iMaxOneMagazine = 1;						// 한 탄창의 탄약 수
	m_iCurrentMagazine = 1;						// 현재 탄창의 탄약 수
	m_iMaxAmmo = 15;							// 최대 소지 탄약 수
	m_iCurrentAmmo = 10;						// 현재 소지 탄약 수

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

// Initialize with Clone
HRESULT CM16M203_Grenade::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT CM16M203_Grenade::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);

	return NOERROR;
}

HRESULT CM16M203_Grenade::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	if(!m_bIsThirdPerson){
		if(!m_bIsLateInit){
			CWeapon::LateInitWeapon();
		}

		WeaponControl(_pPicker);
		UpdateWeaponState(_dDeltaTime);

	}
	return CWeapon::UpdateWeapon(_pPicker, _dDeltaTime);
}

HRESULT CM16M203_Grenade::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

HRESULT CM16M203_Grenade::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// 매니지먼트 변수 가져옴
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 발사
	if(eCurrentSet == IDLE){
		if(pManagement->IsMouseButtonDown(CInputDevice::MOUSE_LBUTTON)){
			if(0 < m_iCurrentMagazine){
				m_pFirstPersonModelCom->SetUpAnimationSet(FIRE, 1.5f);
			}
		}
	}

	// 재장전
	if(eCurrentSet == IDLE){
		if(0 == m_iCurrentMagazine
			&& 0 != m_iCurrentAmmo){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_pFirstPersonModelCom->SetUpAnimationSet(RELOAD, 1.5f);
		}
	}

	// 달리기
	if((eCurrentSet == IDLE)
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
	if(eCurrentSet == IDLE){
		if(pManagement->IsKeyUp(DIK_V)){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_pFirstPersonModelCom->SetUpAnimationSet(KNIFE_MELEE1, 1.5f);
		}
	}
	//무기 교체
	if(eCurrentSet == IDLE){
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

HRESULT CM16M203_Grenade::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	switch(eCurrentSet){
	case CM16M203_Grenade::IDLE:
		return NOERROR;
		break;
	case CM16M203_Grenade::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			--m_iCurrentMagazine;
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CM16M203_Grenade::RELOAD:
		if(m_pFirstPersonModelCom->IsFinished()){
			CWeapon::Reload();
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CM16M203_Grenade::GRENADE_TO_BULLET:
		break;
	case CM16M203_Grenade::BULLET_TO_GRENADE:
		break;
	case CM16M203_Grenade::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		break;
	case CM16M203_Grenade::SPRINT_LOOP:
		break;
	case CM16M203_Grenade::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM16M203_Grenade::PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM16M203_Grenade::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			return NOERROR;
		}
		break;
	case CM16M203_Grenade::QUICK_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM16M203_Grenade::QUICK_PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CM16M203_Grenade::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case CM16M203_Grenade::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(QUICK_PULLOUT, 1.5f);
		break;
	case CM16M203_Grenade::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CM16M203_Grenade::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

CM16M203_Grenade * CM16M203_Grenade::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CM16M203_Grenade* pInstance = new CM16M203_Grenade(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CM16M203_Grenade Create Failure");
	}

	return pInstance;
}

CComponent * CM16M203_Grenade::CreateClone(void * _pArgument){
	CComponent* pInstance = new CM16M203_Grenade(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CM16M203_Grenade Create Failure");
	}

	return pInstance;
}

void CM16M203_Grenade::Free(){
	CWeapon::Free();
}

HRESULT CM16M203_Grenade::ResetAnimationSound(){
	return E_NOTIMPL;
}
