#ifndef __STAGE_SCENE_H__
#define __STAGE_SCENE_H__

// Engine 헤더
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
	// 생성자
	explicit CStageScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
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
	// 프리카메라 오브젝트를 추가하는 함수
	HRESULT AddFreeCamera();
	// 지형 오브젝트를 추가하는 함수
	HRESULT AddTerrain();
	// 조명을 추가하는 함수
	HRESULT AddLightDesc();
	// 스태틱 메쉬를 추가하는 함수
	HRESULT AddStaticMesh();
	// 플레이어를 추가하는 함수
	HRESULT AddPlayer();
	// NPC를 추가하는 함수
	HRESULT AddNonPlayer();
	// 스태틱 메쉬를 추가하는 함수
	HRESULT AddStaticMeshLoader();
	// 이펙트를 추가하는 함수
	HRESULT AddEffect();
	// UI를 추가하는 함수
	HRESULT AddUI();

public:
	// 생성 함수(static)
	static CStageScene* Create(LPDIRECT3DDEVICE9 _pGraphicDevice);

public:
	// Free
	virtual void Free();
};

END
#endif // !__STAGE_SCENE_H__

