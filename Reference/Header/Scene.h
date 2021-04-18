#ifndef __SCENE_H__
#define __SCENE_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract: public CBase{
protected:
	// ������
	explicit CScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// �Ҹ���
	virtual ~CScene() = default;
	
	// Get
public:
	// m_pGraphicDevice�� ��ȯ
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
	// �׷��� ��ġ ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	
	// CBase��(��) ���� ��ӵ�
public:
	// Free
	virtual void Free();
};

END

#endif // !__SCENE_H__
