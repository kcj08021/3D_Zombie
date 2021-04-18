#ifndef __STATIC_MESH_LOADER__
#define __STATIC_MESH_LOADER__

#include "Component.h"
#include "Client_Headers.h"

BEGIN(Client)

class CStaticMeshLoader final
	: public CComponent{
private:
	explicit CStaticMeshLoader(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CStaticMeshLoader(const CStaticMeshLoader& _StaticMeshLoader);
	virtual ~CStaticMeshLoader() = default;

private:
	virtual HRESULT InitComponent_Proto(const _tchar* _szFilePath, _uint _uiSceneNumber);
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;

public:
	HRESULT CreateStaticMeshObject();

private:
	list<OBJECT_SETTING_INFO> m_listObjectSetting;
	_uint m_uiSceneNumber = 0;


public:
	static CStaticMeshLoader* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath, _uint _uiSceneNumber);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END


#endif // !__STATIC_MESH_LOADER__
