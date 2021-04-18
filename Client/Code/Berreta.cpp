#include "stdafx.h"
#include "..\Header\Client_Headers.h"
#include "..\Header\Berreta.h"

USING(Client)

CBeretta::CBeretta(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CWeapon(_pGraphicDevice){
}

CBeretta::CBeretta(const CBeretta & _Beretta)
	: CWeapon(_Beretta){
}

HRESULT CBeretta::InitComponent_Proto(){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// 1��Ī �� ������Ʈ �߰�
	m_pFirstPersonModelCom = dynamic_cast<CDynamicMesh*>(pManagement->AddCloneComponent(L"viewmodel_beretta_Proto", STAGE_SCENE));
	if(nullptr == m_pFirstPersonModelCom){
		SafeRelease(pManagement);
		return E_FAIL;
	}

	// 3��Ī �� ������Ʈ �߰�
	/*if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pThirdPersonModelCom), L"weapon_Beretta", L"weapon_Beretta_Proto", SCENE_TYPE::STAGE_SCENE)))
	return E_FAIL;*/

	// �ѱ� ���� ����
	m_eWeaponType = CWeapon::PS;				// ����Ÿ��
	m_iMaxOneMagazine = 15;						// �� źâ�� ź�� ��
	m_iCurrentMagazine = 15;					// ���� źâ�� ź�� ��
	m_iMaxAmmo = 60;							// �ִ� ���� ź�� ��
	m_iCurrentAmmo = 30;						// ���� ���� ź�� ��

	pManagement->AddTimer(L"M4M203_Timer");

	SafeRelease(pManagement);

	return CWeapon::InitComponent_Proto();
}

HRESULT CBeretta::InitComponent_Clone(void * _pArgument){
	return CWeapon::InitComponent_Clone(_pArgument);
}

HRESULT CBeretta::LateInitWeapon(){
	if(!m_bIsThirdPerson)
		m_pFirstPersonModelCom->SetUpAnimationSet(FIRST_TIME_PULLOUT, 1.5f);

	return NOERROR;
}

HRESULT CBeretta::UpdateWeapon(CPicker* _pPicker, _double _dDeltaTime){
	if(!m_bIsThirdPerson){
		if(!m_bIsLateInit){
			CWeapon::LateInitWeapon();
		}

		WeaponControl(_pPicker);
		UpdateWeaponState(_dDeltaTime);

	}
	return CWeapon::UpdateWeapon(_pPicker, _dDeltaTime);
}

HRESULT CBeretta::RenderWeapon(){
	return CWeapon::RenderWeapon();;
}

HRESULT CBeretta::WeaponControl(CPicker* _pPicker){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	// �Ŵ�����Ʈ ���� ������
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	// ���
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

	// ������
	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
		if(pManagement->IsKeyDown(DIK_R)
			/*			&& 0 != m_iCurrentAmmo
			&& m_iCurrentMagazine != m_iMaxOneMagazine*/){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_pFirstPersonModelCom->SetUpAnimationSet(RELOAD, 1.5f);
		}
	}

	// ������
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

	// ������Ʈ ����
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

	// ������Ʈ ��
	if(eCurrentSet == SPRINT_LOOP){
		if(pManagement->IsKeyUp(DIK_W)
			|| !(*m_pPlayerFlag & CPlayer::FLAG_SPRINT)){
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_OUT, 1.5f);
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_SPRINT;
		}
	}

	// ��������
	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
		if(pManagement->IsKeyUp(DIK_V)){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_pFirstPersonModelCom->SetUpAnimationSet(KNIFE_MELEE1, 1.5f);
		}
	}
	//���� ��ü
	if(eCurrentSet == IDLE || eCurrentSet == ADS_IDLE){
		if(pManagement->IsKeyDown(DIK_1)
			|| pManagement->IsKeyDown(DIK_2)){
			if(nullptr != m_pPlayerFlag)
				*m_pPlayerFlag &= ~CPlayer::FLAG_ADS;
			m_pFirstPersonModelCom->SetUpAnimationSet(PUTAWAY , 1.5f);
		}
	}

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CBeretta::UpdateWeaponState(_double _dDeltaTime){
	ANIMATION_SET eCurrentSet = static_cast<ANIMATION_SET>(m_pFirstPersonModelCom->GetCurrentAnaimationSet());

	switch(eCurrentSet){
	case CBeretta::IDLE:
		return NOERROR;
		break;
	case CBeretta::FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			--m_iCurrentMagazine;
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		}
		break;
	case CBeretta::ADS_UP:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case CBeretta::ADS_IDLE:
		return NOERROR;
		break;
	case CBeretta::ADS_FIRE:
		if(m_pFirstPersonModelCom->IsFinished()){
			--m_iCurrentMagazine;
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		}
		break;
	case CBeretta::ADS_DOWN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::RELOAD:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::RELOAD_EMPTY:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::SPRINT_IN:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(SPRINT_LOOP, 1.5f);
		break;
	case CBeretta::SPRINT_LOOP:
		break;
	case CBeretta::SPRINT_OUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::FIRST_TIME_PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(ADS_IDLE, 1.5f);
		break;
	case CBeretta::PULLOUT:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::PUTAWAY:
		if(m_pFirstPersonModelCom->IsFinished()){
			if(!(*m_pPlayerFlag & CPlayer::FLAG_WEAPON_CHANGE)){
				*m_pPlayerFlag |= CPlayer::FLAG_WEAPON_CHANGE;
			}
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
			return NOERROR;
		}
		break;
	case CBeretta::KNIFE_MELEE1:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CBeretta::KNIFE_MELEE2:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(PULLOUT, 1.5f);
		break;
	case CBeretta::NVG_PUTON:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	case CBeretta::NVG_TAKEOFF:
		if(m_pFirstPersonModelCom->IsFinished())
			m_pFirstPersonModelCom->SetUpAnimationSet(IDLE, 1.5f);
		break;
	default:
		break;
	}

	m_pFirstPersonModelCom->PlayAnimationSet(_dDeltaTime);

	return NOERROR;
}

CBeretta * CBeretta::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CBeretta* pInstance = new CBeretta(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CBeretta Create Failure");
	}
	
	return pInstance;
}

CComponent * CBeretta::CreateClone(void * _pArgument){
	CComponent* pInstance = new CBeretta(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CBeretta Clone Failure");
	}

	return pInstance;
}

void CBeretta::Free(){
	CWeapon::Free();
}
