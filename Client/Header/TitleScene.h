#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

// Client 헤더
#include "..\Header\Client_Headers.h"

// Engine 헤더
#include "Scene.h"

BEGIN(Client)
class CGameStartButton;
class CGameExitButton;

class CTitleScene final: public CScene{
private:
	// 생성자
	explicit CTitleScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
	virtual ~CTitleScene() = default;


private:
	// Initialize
	virtual HRESULT InitScene();
public:
	// Update
	virtual HRESULT UpdateScene(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateScene(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderScene();


private:
	HRESULT AddUIBackground();
	HRESULT AddGameStartButton();
	HRESULT AddGameExitButton();

private:
	void ChangeScene();

private:
	CGameExitButton* m_pExitButton = nullptr;
	CGameExitButton* m_pStartButton = nullptr;

public:
	// 생성 함수(static)
	static CTitleScene* Create(LPDIRECT3DDEVICE9 _pGraphicDevice);

public:
	// Free
	virtual void Free();

};

END

#endif // !__TITLE_SCENE_H__
