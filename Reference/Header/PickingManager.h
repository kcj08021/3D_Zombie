#ifndef __PICKING_MANAGER_H__
#define __PICKING_MANAGER_H__

#include "Base.h"
#include "Pickee.h"
#include "Picker.h"

BEGIN(Engine)

class CGameObject;

class CPickingManager final:
	public CBase{
	DECLARE_SINGLETON(CPickingManager)


private:
	// ������
	explicit CPickingManager();
	// �Ҹ���
	virtual ~CPickingManager() = default;

public:
	// ��ŷ ��� �߰�
	HRESULT AddPickee(CPickee::PICKEE_DESC* _pPickeeDesc);
	// ��ŷ ��� ���� ����
	HRESULT ChangePickeeDesc(CPickee::PICKEE_DESC* _pPickeeDesc);
	// ��ŷ ��� ����
	HRESULT RemovePickeeDesc(CPickee::PICKEE_DESC* _pPickeeDesc);

public:
	// ��ŷ üũ(���� ���Ϳ� ��ġ ����)
	_bool CheckPicking(const _vec3& _vDirection, const _vec3& _vPostion, CPicker::PICK_RESULT_LIST& _listResult);

private:
	// ��ŷ ������ �����ϱ� ���� �����̳�
	list<CPickee::PICKEE_DESC> m_PickeeList;
	typedef	list<CPickee::PICKEE_DESC> PICKEE_LIST;

private:
	// ������ ��ŷ�� üũ�ϴ� �Լ�(Ư�� ��� ���� �����̳� ��ü ��ȸ�ϸ� �˻�)
	_bool DoCheckingPick(const _vec3 & _vRayDir, const _vec3 & _vRayPos, CPicker::PICK_RESULT_LIST& _listResult);

public:
	virtual void Free();
};

END

#endif // !__PICKING_MANAGER_H__