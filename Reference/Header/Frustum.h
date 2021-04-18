#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "Component.h"

BEGIN(Engine)
class CTransform;
class CPipeline;

class ENGINE_DLL CFrustum final:
	public CComponent{
private:
	explicit CFrustum(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual ~CFrustum() = default;

private:
	virtual HRESULT InitComponent_Proto();
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;

public:
	_bool CheckFrustum(const _matrix* _pWorldMatrix, _float _fRadius = 0.f);

private:
	_vec3 m_vProjPoint[8];
	CPipeline* m_pPipeline = nullptr;
	D3DXPLANE m_Plane[6];

private:
	HRESULT SettingFrustum(const _matrix* _pWorldMatrix, _vec3* _pLocalPos);
	
public:
	static CFrustum* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual CComponent * CreateClone(void * _pArgument) override;
public:
	virtual void Free();
};

END

#endif // !__FRUSTUM_H__
