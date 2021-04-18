#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)
class CNavigation;

class ENGINE_DLL CTransform final: 
	public CComponent{
public:
	// ���� ��ȯ ����� ���� ����ü
	enum STATE_TYPE{ STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION };
	// ������ ��ü ���μ��� ����ü(pArgument�� �޾ƿ� ����ü)
	typedef struct tagTransformDesc{
		_matrix*	pMatrixWorldSetting;
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;
private:
	// ������
	explicit CTransform(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CTransform(const CTransform& _Transform);
	// �Ҹ���
	virtual ~CTransform() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

	// Get
public:
	// ���� ��ȯ ����� ���¸� ����
	_vec3 GetState(STATE_TYPE _eState) const{
		return _vec3(m_matWorld.m[_eState]);
	}
	// ���� ��ȯ ����� ����
	_matrix GetWorldMatrix() const{
		return m_matWorld;
	}
	_matrix* GetWorldMatrixPointer(){
		return &m_matWorld;
	}
	// ���� ��ȯ ����� ������� ����
	_matrix GetWorldMatrixInverse() const;

	// Set
public:
	// ���� ��ȯ ��Ŀ� ���¸� ����
	void SetState(STATE_TYPE _eState, const _vec3& vStateData){
		::memcpy(m_matWorld.m[_eState], &vStateData, sizeof(_vec3));
	}
	// ���庯ȯ ��� ��ü
	void SetWorldMatrix(const _matrix& _matWorld){
		m_matWorld = _matWorld;
	}
	// ���μ��� ����
	void SetTransformDesc(const TRANSFORM_DESC& _tTransformDesc){
		m_TransformDecs = _tTransformDesc;
	}

public:
	// Ư�� �������� �̵�
	HRESULT Move(_double _dDeltaTime, _vec3& _vDir, CNavigation* _pNavigation = nullptr);
	// ������ �̵�
	HRESULT MoveStraight(_double _dDeltaTime);
	// �ڷ� �̵�
	HRESULT MoveBackward(_double _dDeltaTime);
	// �·� �̵�
	HRESULT MoveLeft(_double _dDeltaTime);
	// ��� �̵�
	HRESULT MoveRight(_double _dDeltaTime);
	// Right ���� �������� ȸ��
	HRESULT RotationRightAxis(_double _dDeltaTime);
	// Ư�� ���� �������� ȸ��
	HRESULT RotationAxis(const _vec3* _pAxis, _double _dDeltaTime);

private:
	// ���� ��ȯ ���
	_matrix m_matWorld;
	// ������ ��ü ���μ��� ����ü(������ �ʱ�ȭ �ؾ���)
	TRANSFORM_DESC m_TransformDecs;

	_float fRotateRightAxisAcc = 0.f;

public:
	// ���� ���� �Լ�(static)
	static CTransform* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};
END

#endif // !__TRANSFORM_H__


