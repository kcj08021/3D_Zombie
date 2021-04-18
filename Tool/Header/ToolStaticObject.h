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
	// 텍스쳐 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;
	// 스태틱 메쉬 컴포넌트
	CStaticMesh* m_pStaticMeshCom = nullptr;
	// 픽키 컴포넌트
	CPickee* m_pPickeeCom = nullptr;
	// 오브젝트 정보 구조체
	OBJECT_SETTING_INFO m_tSettingInfo;

	const _tchar* m_szLayer = nullptr;

public:
	static CToolStaticObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// Clone(프로토타입을 통한 객체 생성을 위한 함수)
	virtual CGameObject* CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__TOOL_TATIC_OBJECT_H__
