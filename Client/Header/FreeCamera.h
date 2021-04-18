#ifndef __FREE_CAMERA_H__
#define __FREE_CAMERA_H__

// Engine 헤더
#include "Camera.h"

BEGIN(Client)

class CFreeCamera final: 
	public CCamera{

private:
	// 생성자
	explicit CFreeCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CFreeCamera(const CFreeCamera& _FreeCamera);
	// 소멸자
	virtual ~CFreeCamera() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

public:
	// 원본 생성 함수(static)
	static CFreeCamera* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject * CreateClone(void* _pArgument) override;
public:
	// Free
	virtual void Free();
};

END
#endif // !__FREE_CAMERA_H__
