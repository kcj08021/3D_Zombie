#ifndef __SCENE_H__
#define __SCENE_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract: public CBase{
protected:
	// 생성자
	explicit CScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
	virtual ~CScene() = default;
	
	// Get
public:
	// m_pGraphicDevice을 반환
	LPDIRECT3DDEVICE9 GetGraphicDevice(){
		return m_pGraphicDevice;
	}

	// Set
public:

public:
	// Initialize
	virtual HRESULT InitScene() PURE;
	// Update
	virtual HRESULT UpdateScene(_double _dDeltaTime) PURE;
	// LateUpdate
	virtual HRESULT LateUpdateScene(_double _dDeltaTime) PURE;
	// Render
	virtual HRESULT RenderScene() PURE;

private:
	// 그래픽 장치 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	
	// CBase을(를) 통해 상속됨
public:
	// Free
	virtual void Free();
};

END

#endif // !__SCENE_H__
