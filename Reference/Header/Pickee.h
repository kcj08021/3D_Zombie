#ifndef __PICKEE_H__
#define __PICKEE_H__

#include "Component.h"
BEGIN(Engine)
class CTransform;
class CGameObject;

class ENGINE_DLL CPickee final:
	public CComponent{
public:
	// ��ŷ ����� ���μ�����(pArgument)
	enum PICKEE_TYPE{
		TYPE_TERRAIN, TYPE_OBJECT, TYPE_END
	};
	typedef struct tagPickeeDesc{
		// ��ŷ ��� ��ü�� �ּ�
		CGameObject* pGameObj = nullptr;
		union{
			// ������(���� 3��) ��ġ�� �迭
			const POLYGON_POSITION* pPolygonPosition;
			// �޽��� ������
			LPD3DXMESH pMesh;
		};
		// �������� �׷����� �������� ����(0�̸� �޽�)
		_uint uiPrimitiveCount = 0;
		// ��ŷ ����� Ʈ������ ������Ʈ�� ������
		const _matrix* pWolrdMatrix = nullptr;
	}PICKEE_DESC;


private:
	// ������
	explicit CPickee(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CPickee(const CPickee& _Pickee);
	// �Ҹ���
	virtual ~CPickee() = default;


private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);


public:
	HRESULT ChangePickeeDesc(PICKEE_DESC* _pPickeeDesc);
	HRESULT RemovePickeeDesc();

private:
	PICKEE_DESC m_tPickeeDesc;

public:
	// ���� ���� �Լ�(static)
	static CPickee* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};
END
#endif // !__PICKEE_H__
