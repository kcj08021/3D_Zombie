#include "stdafx.h"
#include "..\Header\GameStartButton.h"

#include "..\Header\Client_Headers.h"

#include "Management.h"

USING(Client)

CGameStartButton::CGameStartButton(LPDIRECT3DDEVICE9 _pGraphicDevice)
	:CTextUI(_pGraphicDevice){
}

CGameStartButton::CGameStartButton(const CGameStartButton & _GameStartButton)
	: CTextUI(_GameStartButton){
}

HRESULT CGameStartButton::InitGameObject_Proto(D3DXFONT_DESC * _pDesc){
	return CTextUI::InitGameObject_Proto(_pDesc);

}

HRESULT CGameStartButton::InitGameObject_Clone(void * _pArgument){
	return CTextUI::InitGameObject_Clone(_pArgument);
}

HRESULT CGameStartButton::LateInitGameObject(){
	lstrcpy(m_pString, L"게임 시작");

	return NOERROR;
}

HRESULT CGameStartButton::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	return CTextUI::UpdateGameObject(_dDeltaTime);
}

HRESULT CGameStartButton::LateUpdateGameObject(_double _dDeltaTime){
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

	return CTextUI::LateUpdateGameObject(_dDeltaTime);
}

HRESULT CGameStartButton::RenderGameObject(_double _dDeltaTime){
	return CTextUI::RenderGameObject(_dDeltaTime);
}

CGameStartButton * CGameStartButton::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, D3DXFONT_DESC * _pDesc){
	CGameStartButton* pInstance = new CGameStartButton(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pDesc))){
		MSG_BOX(L"CAmmoHUD Create Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

CGameObject * CGameStartButton::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CGameStartButton(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		MSG_BOX(L"CAmmoHUD Clone Failure");
		SafeRelease(pInstance);
	}
	return pInstance;
}

void CGameStartButton::Free(){
	CTextUI::Free();
}
