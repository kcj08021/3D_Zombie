#ifndef __STATIC_MESH_H__
#define __STATIC_MESH_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh final:
	public CComponent{
public:
	explicit CStaticMesh(LPDIRECT3DDEVICE9 _pGrahpicDevice);
	explicit CStaticMesh(const CStaticMesh& _StaticMesh);
	virtual ~CStaticMesh() = default;

private:
	virtual HRESULT InitComponent_Proto(const _tchar* _szMeshPath, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix);
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;
	
	// Get
public:
	LPD3DXMESH GetMesh(){
		return m_pStaticMesh;
	}
	LPDIRECT3DTEXTURE9 GetMaterialTexture(_uint _uiSubsetIndex, MESH_TEXTURE::TEXTURE_TYPE eType) const;
	_uint GetSubsetCount(){
		return m_dwMaterialCount;
	}
	HRESULT GetBoundaryBox(_vec3& _vMin, _vec3& _vMax);
	HRESULT GetBoundarySphere(_vec3& _vCenter, _float _fRadius);

public:
	HRESULT RenderMesh(_uint _uiSubsetIndex);

private:
	LPD3DXMESH m_pStaticMesh = nullptr;
	LPD3DXBUFFER m_pAdjacencyBuffer = nullptr;
	LPD3DXBUFFER m_pMaterialBuffer = nullptr;
	DWORD m_dwMaterialCount = 0;
	D3DXMATERIAL* m_pMaterial = nullptr;
	MESH_TEXTURE* m_pMeshTexture = nullptr;
	_uint m_uiStride = 0;

private:
	HRESULT ChangeTextureFileName(_tchar* _szOut, const _tchar* _szIn, const _tchar* _szSrcText, const _tchar* _szDestText);

public:
	static CStaticMesh* CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar * _szMeshDir, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix = nullptr);
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__STATIC_MESH_H__
