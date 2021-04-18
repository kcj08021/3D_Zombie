#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "Component.h"

BEGIN(Engine)

class CShader;
class CTransform;
class CPipeline;
class CCollisionManager;
class CGameObject;

class ENGINE_DLL CCollider final :
	public CComponent{
public:
	// �浹 Ÿ�� ����ü
	enum COLL_TYPE{
		COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE
	};
	// �ݶ��̴� ���μ��� ����ü, ���浹 �ݶ��̴��� ��� x�� �д´�.
	typedef struct tagColliderDesc{
		CGameObject* pGameObj;
		_vec3 vScale;
		_vec3 vLocalPosition;
		_matrix* pParentWorldMatrix;
	}COLLIDER_DESC;
	// OBB�� üũ�ϱ� �� ������ ��� ���� ����ü
	typedef struct tagOBBDesc{
		_vec3 vProjAxis[3];
		_vec3 vAlignAxis[3];
		_vec3 vCenterPos;
	}OBB_DESC;
private:
	// ������
	explicit CCollider(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CCollider(const CCollider& _Collider);
	// �Ҹ���
	virtual ~CCollider() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(COLL_TYPE _eType);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void * _pArgument) override;

public:

public:
	// �浹 �Ŵ����� �ݶ��̴� ���
	HRESULT SettingToCollisionManager();
	// �ݶ��̴��� ������Ʈ
	HRESULT UpdateCollider();
	// �ݶ��̴��� ����
	HRESULT RenderCollider();
	// �浹 �Ŵ������� �ݶ��̴� ����
	HRESULT DeleteToCollisionManager();
	// �浹 üũ
	_bool IsCollision();

	// Get
public:
	_uint GetColliderType(){
		return m_eType;
	}
	_bool IsEnable(){
		return m_bIsEnable;
	}
	LPD3DXMESH GetMesh(){
		return m_pMesh;
	}
	_vec3 GetPosition(){
		return _vec3(m_tWorldMatrix.m[3]);
	}
	CGameObject* GetCollisionObject(){
		return m_pCollisionObject;
	}
	const _matrix* GetWorldMatrixPointer() const{
		return &m_tWorldMatrix;
	}

private:
	// �ݶ��̴��� ���̴�
	CShader* m_pColliderShader = nullptr;
	// ��ȯ ��ĵ��� ������ ���� ���������� ����
	CPipeline* m_pPipeline = nullptr;
	// �浹 Ÿ��
	COLL_TYPE m_eType;
	// �浹 ������Ʈ�� ���μ��� ��
	COLLIDER_DESC m_tColliderDesc;
	// �ݶ��̴� �޽�
	LPD3DXMESH m_pMesh = nullptr;
	// �ݶ��̴� �޽��� �������
	LPD3DXBUFFER m_pAdjacency = nullptr;

	// �ݶ��̴��� �������
	_matrix m_tWorldMatrix;

	// AABB�� �浹�� �˻��ϴ� ����
	_vec3 m_vMin_AABB;
	_vec3 m_vMax_AABB;

	// OBB�� �˻��ϱ����� ����ü(���浹�� �ʿ� ������ AABB�� OBB�϶��� ���� �Ҵ�)
	OBB_DESC* m_pOBB_Desc = nullptr;

	// �浹 Ȯ�� ����
	_bool m_bIsCollision = false;
	// �浹�� ��ü
	CGameObject* m_pCollisionObject = nullptr;
	// �浹 �˻� ����
	_bool m_bIsEnable = false;

public:
	static CCollider* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, COLL_TYPE _eType);
	virtual CComponent * CreateClone(void * _pArgument) override;

public:
	virtual void Free();

	friend CCollisionManager;
};

END

#endif // !__COLLIDER_H__