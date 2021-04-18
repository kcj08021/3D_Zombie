#ifndef __TERRAIN_H__
#define __TERRAIN_H__

// Engine 헤더
#include "GameObject.h"

// 전방 선언
BEGIN(Engine)
class CShader;
class CRenderer;
class CStaticMesh;
END

BEGIN(Client)

class CTerrain final:
	public CGameObject{
private:
	// 생성자
	explicit CTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CTerrain(const CTerrain& _Terrain);
	// 소멸자
	virtual ~CTerrain() = default;

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

private:
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 렌더러 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;
	// 스태틱 메쉬 컴포넌트
	CStaticMesh* m_pStaticMesh = nullptr;
	CPickee* m_pPickeeCom = nullptr;

public:
	// 원본 생성 함수(static)
	static CTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__TERRAIN_H__
