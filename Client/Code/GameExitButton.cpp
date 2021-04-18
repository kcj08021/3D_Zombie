#include "stdafx.h"
#include "..\Header\GameExitButton.h"

#include "..\Header\Client_Headers.h"

#include "Management.h"

USING(Client)

CGameExitButton::CGameExitButton(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CTextUI(_pGraphicDevice){
}

CGameExitButton::CGameExitButton(const CGameExitButton & _GameExitButton)
	:CTextUI(_GameExitButton){
}

HRESULT CGameExitButton::InitGameObject_Proto(D3DXFONT_DESC * _pDesc){
	return CTextUI::InitGameObject_Proto(_pDesc);

}

HRESULT CGameExitButton::InitGameObject_Clone(void * _pArgument){
	return CTextUI::InitGameObject_Clone(_pArgument);
}

HRESULT CGameExitButton::LateInitGameObject(){
	lstrcpy(m_pString, L"게임 종료");

	return NOERROR;
}

HRESULT CGameExitButton::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	POINT pt = {};
	::GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);

	if(PtInRect(CTextUI::GetRect(), pt)){
		if(!m_bSound){
			pManagement->PlaySound(L"mouse_ylover.wav", 3);
			m_bSound = true;
		}
		if(pManagement->IsMouseButtonPressing(CInputDevice::MOUSE_LBUTTON))
			m_bIsClick = true;
	} else{
		m_bSound = false;
	}
		

	SafeRelease(pManagement);

	return CTextUI::UpdateGameObject(_dDeltaTime);
}

HRESULT CGameExitButton::LateUpdateGameObject(_double _dDeltaTime){
	return CTextUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CGameExitButton::RenderGameObject(_double _dDeltaTime){
	return CTextUI::RenderGameObject(_dDeltaTime);
}

CGameExitButton * CGameExitButton::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC * _pDesc){
	CGameExitButton* pInstance = new CGameExitButton(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pDesc))){
		MSG_BOX(L"CAmmoHUD Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CGameExitButton::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CGameExitButton(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CAmmoHUD Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CGameExitButton::Free(){
	CTextUI::Free();
}
