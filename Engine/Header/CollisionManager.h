#ifndef __COLLISION_MANAGER_H__
#define __COLLISION_MANAGER_H__
#include "Base.h"
#include "Collider.h"

BEGIN(Engine)

class CCollisionManager final:
	public CBase{
	DECLARE_SINGLETON(CCollisionManager)
private:
	enum COLL_TYPE{
		COLL_BB, COLL_SPHERE, COLL_END
	};

private:
	explicit CCollisionManager();
	virtual ~CCollisionManager() = default;

public:
	HRESULT RenderCollider();

public:
	HRESULT AddCollider(CCollider* _Collider);
	HRESULT RemoveCollider(CCollider* _Collider);

public:
	_bool IsCollision(CCollider* _Collider);

private:
	list<CCollider*> m_ColliderList[COLL_END];
	typedef list<CCollider*> COLL_LIST;

private:
	_bool IsCollision_AABB(CCollider* _pSrc, CCollider* _pDest);
	_bool IsCollision_OBB(CCollider* _pSrc, CCollider* _pDest);
	_bool IsCollision_Sphere(CCollider* _pSrc, CCollider* _pDest);

public:
	virtual void Free();
};

END

#endif // !__COLLISION_MANAGER_H__
