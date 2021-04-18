#ifndef __TOOL_CAMERA_H__
#define __TOOL_CAMERA_H__

#include "Camera.h"

BEGIN(Tool)

class CToolCamera final 
	: public CCamera{
private:
	// ������
	explicit CToolCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CToolCamera(const CToolCamera& _ToolCamera);
	// �Ҹ���
	virtual ~CToolCamera() = default;

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
	virtual HRESULT RenderGameObject();

public:
	HRESULT SetFocusPos(const _vec3& _vPos);

public:
	// ���� ���� �Լ�(static)
	static CToolCamera* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CGameObject * CreateClone(void * _pArgument) override;

public:
	virtual void Free();

};

END

#endif // !__TOOL_CAMERA_H__

