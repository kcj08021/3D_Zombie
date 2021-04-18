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
	// 생성자
	explicit CPickingManager();
	// 소멸자
	virtual ~CPickingManager() = default;

public:
	// 피킹 대상 추가
	HRESULT AddPickee(CPickee::PICKEE_DESC* _pPickeeDesc);
	// 피킹 대상 정보 변경
	HRESULT ChangePickeeDesc(CPickee::PICKEE_DESC* _pPickeeDesc);
	// 피킹 대상 제거
	HRESULT RemovePickeeDesc(CPickee::PICKEE_DESC* _pPickeeDesc);

public:
	// 피킹 체크(방향 벡터와 위치 벡터)
	_bool CheckPicking(const _vec3& _vDirection, const _vec3& _vPostion, CPicker::PICK_RESULT_LIST& _listResult);

private:
	// 피킹 대상들을 보관하기 위한 컨테이너
	list<CPickee::PICKEE_DESC> m_PickeeList;
	typedef	list<CPickee::PICKEE_DESC> PICKEE_LIST;

private:
	// 실제로 피킹을 체크하는 함수(특정 대상 없이 컨테이너 전체 순회하며 검사)
	_bool DoCheckingPick(const _vec3 & _vRayDir, const _vec3 & _vRayPos, CPicker::PICK_RESULT_LIST& _listResult);

public:
	virtual void Free();
};

END

#endif // !__PICKING_MANAGER_H__