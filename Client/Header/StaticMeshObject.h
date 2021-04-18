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
	// ������
	explicit CStaticMeshObject(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	// ���� ������
	explicit CStaticMeshObject(const CStaticMeshObject& _StaticMeshObject);
	// �Ҹ���
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
	// �ؽ��� ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;
	// ����ƽ �޽� ������Ʈ
	CStaticMesh* m_pStaticMeshCom = nullptr;
	// �ݶ��̾� ������Ʈ
	CCollider* m_pColliderCom = nullptr;
	// ����ü ������Ʈ
	CFrustum* m_pFrustumCom = nullptr;
	// ��Ű ������Ʈ
	CPickee* m_pPickeeCom = nullptr;
	// ������Ʈ�� �߽���
	_vec3 m_vCenter;
	// ����ü �ø� ��� �Ÿ�(������)
	_float m_fCullRadius = 0.f;

public:
	static CStaticMeshObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__STATIC_MESH_OBJECT_H__



