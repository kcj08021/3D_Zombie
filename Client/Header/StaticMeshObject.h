#ifndef __STATIC_MESH_OBJECT_H__
#define __STATIC_MESH_OBJECT_H__

#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CStaticMeshObject final:
	public CGameObject{
public:
	typedef struct tagStaticMeshDesc{
		CTransform::TRANSFORM_DESC tTransformDesc;
		const _tchar* szStaticMeshComName;
	}STATIC_MESH_DESC;

private:
	// 생성자
	explicit CStaticMeshObject(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	// 복사 생성자
	explicit CStaticMeshObject(const CStaticMeshObject& _StaticMeshObject);
	// 소멸자
	virtual ~CStaticMeshObject() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) override;

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
	// 텍스쳐 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;
	// 스태틱 메쉬 컴포넌트
	CStaticMesh* m_pStaticMeshCom = nullptr;
	// 콜라이어 컴포넌트
	CCollider* m_pColliderCom = nullptr;
	// 절두체 컴포넌트
	CFrustum* m_pFrustumCom = nullptr;
	// 픽키 컴포넌트
	CPickee* m_pPickeeCom = nullptr;
	// 오브젝트의 중심점
	_vec3 m_vCenter;
	// 절두체 컬링 허용 거리(반지름)
	_float m_fCullRadius = 0.f;

public:
	static CStaticMeshObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__STATIC_MESH_OBJECT_H__



