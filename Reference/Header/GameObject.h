#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Base.h"
#include "..\Header\ComponentManager.h"

BEGIN(Engine)

//���� ����
class CComponent;

class ENGINE_DLL CGameObject abstract: 
	public CBase{
protected:
	// ������
	explicit CGameObject(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CGameObject(const CGameObject& _pObj);
	// �Ҹ���
	virtual ~CGameObject() = default;

	// Get
public:
	// ��ü���� ������� ������Ʈ�� �˻�, ��ȯ
	CComponent* GetComponent(const _tchar* _szKey){
		return FindComponent(_szKey);
	}
	// �׷��� ��ġ�� ������ �Լ�
	LPDIRECT3DDEVICE9 GetGraphicDevice() const{
		return m_pGraphicDevice;
	}
	// �� �����̽����� Z���� ������ �Լ�
	_float GetViewZ(){
		return m_fViewZ;
	}
	_bool GetIsDeadObject(){
		return m_bIsDeadObject;
	}
public:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) PURE;
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

protected: 
	// ������Ʈ�� ���� �� �����̳ʿ� �����ϱ� ���� �Լ�
	HRESULT AddComponent(CComponent** _ppOutComponent, const _tchar* _szCloneKey, const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument = nullptr );
	// ���� Ư�� ������Ʈ�� ����� �Լ�
	HRESULT ReleaseComponent(CComponent** _ppComponent, const _tchar* _szCloneKey);
	// �� �����̽����� Z���� �����
	HRESULT ComputeViewZ(_vec3 _vWorldPos);

protected:
	// LateInitialize Ȯ�� ����
	_bool m_bIsLateInit = false;
	_bool m_bIsDeadObject = false;

private:
	// ������Ʈ �����̳ʿ��� �˻��ϴ� �Լ�
	CComponent* FindComponent(const _tchar* _szFindString);

private:
	// �׷��� ��ġ ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// ������Ʈ�� ������ �����̳�(�ܺο��� �˻��ϱ� ����)
	unordered_map<const _tchar*, CComponent*> m_ComponentMap;
	typedef unordered_map<const _tchar*, CComponent*> COMPONENT_MAP;
	// �� �����̽� �󿡼��� Z��(���� ���� ��ü�� �ѿ��� �����)
	_float m_fViewZ = 0.f;
	
public:
	// Clone(������Ÿ���� ���� ��ü ������ ���� �Լ�)
	virtual CGameObject* CreateClone(void* _pArgument) PURE;

	// CBase��(��) ���� ��ӵ�
public:
	// Free
	virtual void Free() override;
};

END
#endif // !__GAME_OBJECT_H__

