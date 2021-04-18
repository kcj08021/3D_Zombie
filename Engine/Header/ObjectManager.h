#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

// 전방 선언
class CLayer;

class CObjectManager final: public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CObjectManager)
private:
	// 생성자
	explicit CObjectManager();
	// 소멸자
	virtual ~CObjectManager() = default;

public:
	list<CGameObject*>* GetObjectList(_uint _uiIndex, const _tchar* _szLayer);

public:
	// 씬 갯수에 맞춰서 레어이맵 배열 동적 할당
	HRESULT ReserveLayerMapArray(_uint _uiSceneCount);
	// Update
	HRESULT UpdateObjectMgr(_double _dDeltaTime);
	// LateUpdate
	HRESULT LateUpdateObjectMgr(_double _dDeltaTime);
	// 사용이 끝난 씬 할당 해제
	HRESULT SceneClear(_uint _uiScene);

	/* 오브젝트 추가 및 삭제 관련 */
public:
	// 프로토타입 오브젝트 추가
	HRESULT AddProtoObject(const _tchar* _szKey,CGameObject* _pObj);
	// 프로토타입을 복제하여 레이어에 추가
	HRESULT AddCloneObjectToLayer(const _tchar* _szProtoKey, _uint _uiIndex, const _tchar* _szLayerKey, void* _pArgument , CGameObject** _ppOutClone);
	// 레이어 삭제
	HRESULT ReleaseLayer(_uint _uiIndex, const _tchar* _szLayerKey);
	// 레이어 안의 복제 오브젝트 삭제
	HRESULT ReleaseCloneObject(_uint _uiIndex, const _tchar* _szLayerKey, CGameObject* _pSelectedObj);

private:
	// 프로토타입 오브젝트가 담기는 컨테이너 변수(unordered_map)
	unordered_map<const _tchar*, CGameObject*> m_PrototypeMap;
	// 타입 재정의
	typedef unordered_map<const _tchar*, CGameObject*> PROTO_MAP;

	// 레이어 배열의 크기(==씬의 갯수)를 담는 변수
	_uint m_uiSceneCount = 0;
	// 프로토타입을 복제한 오브젝트가 담기는 컨테이너 변수(unordered_map)
	unordered_map<const _tchar*, CLayer*>* m_pLayerMapArray = nullptr;
	// 타입 재정의
	typedef unordered_map<const _tchar*, CLayer*> LAYER_MAP;

private:
	// 프로토타입 컨테이너 내부 검색
	CGameObject* FindPrototype(const _tchar* _szFindString);
	// 레이어 컨테이너 내부 검색
	CLayer* FindLayer(_uint _uiIndex ,const _tchar* _szFindString);

public:
	// Free
	virtual void Free();
};

END

#endif // !__OBJECT_MANAGER_H__
