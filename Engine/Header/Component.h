#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CComponent abstract: public CBase{
protected:
	// ������
	explicit CComponent(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CComponent(const CComponent& _Component);
	// �Ҹ���
	virtual ~CComponent() = default;

	// Get
public:
	LPDIRECT3DDEVICE9 GetGraphicDevice(){
		return m_pGraphicDevice;
	}

public:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) PURE;

protected:
	_bool IsClone(){
		return m_bIsClone;
	}

private:
	// �׷��� ��ġ ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// ������ ������Ʈ���� Ȯ���ϴ� bool ����
	_bool m_bIsClone = false;

public:
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument) PURE;
	// Free
	virtual void Free();
};

END

#endif // !__COMPONENT_H__

