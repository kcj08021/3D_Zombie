#ifndef __TOOL_TATIC_OBJECT_H__
#define __TOOL_TATIC_OBJECT_H__

#include "GameObject.h"
#include "..\Header\Tool_Struct.h"
BEGIN(Tool)


class CToolStaticObject final : 
	public CGameObject{
public:
	typedef struct tagStaticMeshDesc{
		OBJECT_SETTING_INFO tSettingInfo;
		const _tchar* szLayer;
	}STATIC_MESH_DESC;

private:
	explicit CToolStaticObject(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CToolStaticObject(const CToolStaticObject& _ToolStaticObject);
	virtual ~CToolStaticObject() = default;

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

public:
	LPD3DXMESH GetMesh(){
		return m_pStaticMeshCom->GetMesh();
	}
	_vec3 GetPos(){
		return _vec3(m_pTransformCom->GetWorldMatrixPointer()->m[3]);
	}
	const OBJECT_SETTING_INFO& GetInfo(){
		return m_tSettingInfo;
	}

public:
	HRESULT SetInfo(OBJECT_SETTING_INFO& _Info);


private:
	// �ؽ��� ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;
	// ����ƽ �޽� ������Ʈ
	CStaticMesh* m_pStaticMeshCom = nullptr;
	// ��Ű ������Ʈ
	CPickee* m_pPickeeCom = nullptr;
	// ������Ʈ ���� ����ü
	OBJECT_SETTING_INFO m_tSettingInfo;

	const _tchar* m_szLayer = nullptr;

public:
	static CToolStaticObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// Clone(������Ÿ���� ���� ��ü ������ ���� �Լ�)
	virtual CGameObject* CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__TOOL_TATIC_OBJECT_H__
