#ifndef __FREE_CAMERA_H__
#define __FREE_CAMERA_H__

// Engine ���
#include "Camera.h"

BEGIN(Client)

class CFreeCamera final: 
	public CCamera{

private:
	// ������
	explicit CFreeCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CFreeCamera(const CFreeCamera& _FreeCamera);
	// �Ҹ���
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
	// ���� ���� �Լ�(static)
	static CFreeCamera* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CGameObject * CreateClone(void* _pArgument) override;
public:
	// Free
	virtual void Free();
};

END
#endif // !__FREE_CAMERA_H__
