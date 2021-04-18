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
	// ������
	explicit CPicker(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CPicker(const CPicker& _Picker);
	// �Ҹ���
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
	// ��ŷ üũ(���콺 ��ǥ)
	_bool CheckPicking(const POINT& _tMousePt, _vec3* _pOutLocation = nullptr);
	// ��ŷ üũ(���⺤�Ϳ� ��ġ)
	_bool CheckPicking(const _vec3& _vDirection, const _vec3& _vPostion, _vec3* _pOutLocation = nullptr);
	// ���콺 ��ǥ�� ���� �����̽����� ��ȯ�ϴ� �Լ�
	HRESULT TransformMousePos(const POINT & _tInMousePos, _vec3& _vOutRayDir, _vec3& _vOutRayPos);

private:
	// ��ŷ ����� �����ϴ� ����Ʈ
	list<CPicker::PickingResult> m_ListPickingResult;
	// ��ŷ �Ŵ��� ����
	CPickingManager* m_pPickingManager = nullptr;

private:
	_vec3 GetFirstIntersectPoint(const _vec3 & _vDirection, const _vec3& _vPostion);

public:
	typedef list<CPicker::PickingResult> PICK_RESULT_LIST;


public:
	// ���� ���� �Լ�(static)
	static CPicker* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};

END;

#endif // !__PICKER_H__
