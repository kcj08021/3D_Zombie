#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

// ���� ����
class CLayer;

class CObjectManager final: public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CObjectManager)
private:
	// ������
	explicit CObjectManager();
	// �Ҹ���
	virtual ~CObjectManager() = default;

public:
	list<CGameObject*>* GetObjectList(_uint _uiIndex, const _tchar* _szLayer);

public:
	// �� ������ ���缭 �����̸� �迭 ���� �Ҵ�
	HRESULT ReserveLayerMapArray(_uint _uiSceneCount);
	// Update
	HRESULT UpdateObjectMgr(_double _dDeltaTime);
	// LateUpdate
	HRESULT LateUpdateObjectMgr(_double _dDeltaTime);
	// ����� ���� �� �Ҵ� ����
	HRESULT SceneClear(_uint _uiScene);

	/* ������Ʈ �߰� �� ���� ���� */
public:
	// ������Ÿ�� ������Ʈ �߰�
	HRESULT AddProtoObject(const _tchar* _szKey,CGameObject* _pObj);
	// ������Ÿ���� �����Ͽ� ���̾ �߰�
	HRESULT AddCloneObjectToLayer(const _tchar* _szProtoKey, _uint _uiIndex, const _tchar* _szLayerKey, void* _pArgument , CGameObject** _ppOutClone);
	// ���̾� ����
	HRESULT ReleaseLayer(_uint _uiIndex, const _tchar* _szLayerKey);
	// ���̾� ���� ���� ������Ʈ ����
	HRESULT ReleaseCloneObject(_uint _uiIndex, const _tchar* _szLayerKey, CGameObject* _pSelectedObj);

private:
	// ������Ÿ�� ������Ʈ�� ���� �����̳� ����(unordered_map)
	unordered_map<const _tchar*, CGameObject*> m_PrototypeMap;
	// Ÿ�� ������
	typedef unordered_map<const _tchar*, CGameObject*> PROTO_MAP;

	// ���̾� �迭�� ũ��(==���� ����)�� ��� ����
	_uint m_uiSceneCount = 0;
	// ������Ÿ���� ������ ������Ʈ�� ���� �����̳� ����(unordered_map)
	unordered_map<const _tchar*, CLayer*>* m_pLayerMapArray = nullptr;
	// Ÿ�� ������
	typedef unordered_map<const _tchar*, CLayer*> LAYER_MAP;

private:
	// ������Ÿ�� �����̳� ���� �˻�
	CGameObject* FindPrototype(const _tchar* _szFindString);
	// ���̾� �����̳� ���� �˻�
	CLayer* FindLayer(_uint _uiIndex ,const _tchar* _szFindString);

public:
	// Free
	virtual void Free();
};

END

#endif // !__OBJECT_MANAGER_H__
