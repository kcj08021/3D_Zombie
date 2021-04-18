#ifndef __STAGE_SCENE_H__
#define __STAGE_SCENE_H__

// Engine ���
#include "Scene.h"

BEGIN(Engine)
class CNavigation;
class CManagement;
END

BEGIN(Client)
class CTerrain;
class CStaticMeshObject;
class CStaticMeshLoader;
class CUnitManager;

class CStageScene:
	public CScene{
private:
	// ������
	explicit CStageScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// �Ҹ���
	virtual ~CStageScene() = default;

private:
	// Initialize
	virtual HRESULT InitScene() override;
public:
	// Update
	virtual HRESULT UpdateScene(_double _dDeltaTime) override;
	// LateUpdate
	virtual HRESULT LateUpdateScene(_double _dDeltaTime) override;
	// Render
	virtual HRESULT RenderScene() override;

private:
	CTerrain* m_pSceneMap = nullptr;
	_bool m_bIsDebug = false;
	CNavigation* m_pNavigationCom = nullptr;
	CStaticMeshLoader* m_pStaticMeshLoader = nullptr;
	CUnitManager* m_pUnitManager = nullptr;
private:
	// ����ī�޶� ������Ʈ�� �߰��ϴ� �Լ�
	HRESULT AddFreeCamera();
	// ���� ������Ʈ�� �߰��ϴ� �Լ�
	HRESULT AddTerrain();
	// ������ �߰��ϴ� �Լ�
	HRESULT AddLightDesc();
	// ����ƽ �޽��� �߰��ϴ� �Լ�
	HRESULT AddStaticMesh();
	// �÷��̾ �߰��ϴ� �Լ�
	HRESULT AddPlayer();
	// NPC�� �߰��ϴ� �Լ�
	HRESULT AddNonPlayer();
	// ����ƽ �޽��� �߰��ϴ� �Լ�
	HRESULT AddStaticMeshLoader();
	// ����Ʈ�� �߰��ϴ� �Լ�
	HRESULT AddEffect();
	// UI�� �߰��ϴ� �Լ�
	HRESULT AddUI();

public:
	// ���� �Լ�(static)
	static CStageScene* Create(LPDIRECT3DDEVICE9 _pGraphicDevice);

public:
	// Free
	virtual void Free();
};

END
#endif // !__STAGE_SCENE_H__

