#ifndef __GRAVITY_H__
#define __GRAVITY_H__

#include "Component.h"

BEGIN(Engine)
class CStaticMesh;
class CTransform;
END

BEGIN(Client)

class CGravity final
	: CComponent{
private:
	explicit CGravity(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	explicit CGravity(const CGravity& _Gravity);
	virtual ~CGravity() = default;

private:
	virtual HRESULT InitComponent_Proto(const CStaticMesh* _pStaticMesh);
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;
	
public:
	HRESULT CheckGround(CTransform* _pTransform);

private:
	const CStaticMesh* m_pTerrainStaticMesh = nullptr;

public:
	static CGravity* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce, const CStaticMesh* pStaticMesh);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();
};

END


#endif // !__GRAVITY_H__
