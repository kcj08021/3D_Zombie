#ifndef __NAVIMESH_OBJECT_H__
#define __NAVIMESH_OBJECT_H__

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Tool)
class CToolStaticObject;

class CNaviMeshObject final : public CGameObject{
private:
	explicit CNaviMeshObject(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CNaviMeshObject(const CNaviMeshObject& _NaviMeshObject);
	virtual ~CNaviMeshObject() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(CListBox* _pListBox);
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
	void SetIsNaviMeshCreate(_bool _bIsNaviMeshCreate){
		m_bIsNaviMeshCreate = _bIsNaviMeshCreate;
	}
	void SetIsNaviMeshVisible(_bool _bIsNaviMeshVisible){
		m_bIsNaviMeshVisible = _bIsNaviMeshVisible;
	}
	HRESULT SetPickingPos(_vec3 & _vPos);
	void SetSelectedIndex(_int _iIndex){
		m_iSelectedIndex = _iIndex;
	}

public:
	HRESULT SaveNaviMesh(const _tchar* _szFilePath);
	HRESULT LoadNaviMesh(const _tchar* _szFilePath);
	HRESULT ReleaseNaviMesh(_int _iIndex);

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShaderCom = nullptr;
	_vec3* m_vPoint[3];
	_uint m_uiPointIndex = 0;
	_int m_iSelectedIndex = 0;
	list<POLYGON_POSITION*> m_listPolygon;
	list<CCollider*> m_listCollider;
	list<LPDIRECT3DVERTEXBUFFER9> m_listVertexBuffer;
	CListBox* m_pListBox = nullptr;

	_bool m_bIsNaviMeshCreate = false;
	_bool m_bIsNaviMeshVisible = false;

private:
	HRESULT TrasnformMousePoint(const POINT & _tInMousePos, _vec3& _vOutRayDir, _vec3& _vOutRayPos);

public:
	static CNaviMeshObject* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CListBox* _pListBox);
	virtual CGameObject* CreateClone(void * _pArgument);

public:
	virtual void Free();
};

END

#endif // !__NAVIMESH_OBJECT_H__
