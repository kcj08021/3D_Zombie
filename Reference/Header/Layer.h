#ifndef __LAYER_H__
#define __LAYER_H__

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class CLayer: public CBase{
private:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	list<CGameObject*>* GetList(){
		return &m_ObjectList;
	}

public:
	HRESULT InitLayer();
	HRESULT UpdateLayer(_double _dDeltaTime);
	HRESULT LateUpdateLayer(_double _dDeltaTime);

	HRESULT AddGameObject(CGameObject* _pObj);

	HRESULT ReleaseCloneObject(CGameObject* _pObj);

private:
	list<CGameObject*> m_ObjectList;
	typedef list<CGameObject*> OBJ_LIST;

public:
	virtual void Free();

public:
	static CLayer* Create();
};

END

#endif // !__LAYER_H__
