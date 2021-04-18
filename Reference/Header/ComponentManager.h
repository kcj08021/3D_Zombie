#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include "Base.h"
#include "..\Header\Component.h"

// �������� ������ ������Ʈ��
#include "..\Header\Renderer.h"
#include "..\Header\Shader.h"
#include "..\Header\Texture.h"
#include "..\Header\VIBufferRect.h"
#include "..\Header\VIBufferTerrain.h"
#include "..\Header\VIBufferCube.h"
#include "..\Header\Transform.h"
#include "..\Header\Picker.h"
#include "..\Header\Pickee.h"
#include "..\Header\StaticMesh.h"
#include "..\Header\DynamicMesh.h"
#include "..\Header\Collider.h"
#include "..\Header\Navigation.h"
#include "..\Header\Frustum.h"


BEGIN(Engine)

class CComponentManager final: public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CComponentManager)

private:
	// ������
	explicit CComponentManager();
	// �Ҹ���
	virtual ~CComponentManager() = default;

public:
	// �� ������ ���缭 �����̸� �迭 ���� �Ҵ�
	HRESULT ReserveComponentMapArray(_uint _uiSceneCount);

	//������Ʈ �߰� ����
public:
	// ������Ÿ�� ������Ʈ �߰�
	HRESULT AddProtoComponent(const _tchar* _szKey, _uint _uiIndex, CComponent* _pComponent);
	// ������Ÿ���� �����Ͽ� ��ȯ
	CComponent* AddCloneComponent(const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument);

	// ��ϵǾ� �ִ� ������Ÿ�� ������Ʈ�� ����
	HRESULT ReleaseProtoComponent(const _tchar* _szKey, _uint _uiIndex);

private:
	// ������Ʈ�� ������Ÿ���� �����ϴ� �����̳�(��)�� �迭(�� ���� ����)
	unordered_map<const _tchar*, CComponent*>* m_pComponentMapArray = nullptr;
	typedef unordered_map<const _tchar*, CComponent*> COMPONENT_MAP;
	// �� ���� ����
	_uint m_uiSceneCount = 0;

private:
	// �����̳� �˻� �Լ�
	CComponent* FindPrototype(const _tchar* _szFindString, _uint _uiIndex);

	// CBase��(��) ���� ��ӵ�
public:
	// Free
	virtual void Free();
};

END

#endif // !__COMPONENT_MANAGER_H__


