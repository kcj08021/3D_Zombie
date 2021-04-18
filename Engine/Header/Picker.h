#ifndef __PICKER_H__
#define __PICKER_H__

#include "Component.h"

BEGIN(Engine)
class CPickingManager;
class CGameObject;
class CTransform;

class ENGINE_DLL CPicker final:
	public CComponent{
public:
	typedef struct tagPickingResult{
		CGameObject* pGameObject = nullptr;
		const _matrix* pWolrdMatrix = nullptr;
		_float fU = 0.f;
		_float fV = 0.f;
		_float fDistance = 0.f;
		_float fThickness = 0.f;
}PickingResult;

private:
	// 생성자
	explicit CPicker(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CPicker(const CPicker& _Picker);
	// 소멸자
	virtual ~CPicker() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

public:
	const list<CPicker::PickingResult>& GetPickingResult(){
		return m_ListPickingResult;
	}

public:
	// 피킹 체크(마우스 좌표)
	_bool CheckPicking(const POINT& _tMousePt, _vec3* _pOutLocation = nullptr);
	// 피킹 체크(방향벡터와 위치)
	_bool CheckPicking(const _vec3& _vDirection, const _vec3& _vPostion, _vec3* _pOutLocation = nullptr);
	// 마우스 좌표를 월드 스페이스까지 변환하는 함수
	HRESULT TransformMousePos(const POINT & _tInMousePos, _vec3& _vOutRayDir, _vec3& _vOutRayPos);

private:
	// 피킹 결과를 보관하는 리스트
	list<CPicker::PickingResult> m_ListPickingResult;
	// 피킹 매니저 변수
	CPickingManager* m_pPickingManager = nullptr;

private:
	_vec3 GetFirstIntersectPoint(const _vec3 & _vDirection, const _vec3& _vPostion);

public:
	typedef list<CPicker::PickingResult> PICK_RESULT_LIST;


public:
	// 원본 생성 함수(static)
	static CPicker* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};

END;

#endif // !__PICKER_H__
