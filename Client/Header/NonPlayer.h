#ifndef __NON_PLAYER_H__
#define __NON_PLAYER_H__

#include "GameObject.h"
#include "..\Header\Client_Headers.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)

class CStaticMeshObject;

class CNonPlayer final :
	public CGameObject{
public:
	typedef struct tagNonPlayerDesc{
		const _tchar* szDynamicMeshProtoName;
		const _tchar* szWeapon;
		CTransform::TRANSFORM_DESC tTransformDesc;
	}NONPLAYER_DESC;

private:
	enum PART_TYPE{
		PARENT, HEAD, BODY_UPPER, BDOY_LOWER, LEFT_SHOULDER, LEFT_ELBOW, RIGHT_SHOULDER, RIGHT_ELBOW, LFET_HIP, LEFT_KNEE, RIGHT_HIP, RIGHT_KNEE, PART_END
	};

	enum ANIMATION_SET{
		ZOMBIE_IDLE					       = 0,
		ZOMBIE_ATTACK,					// = 1
		ZOMBIE_WALK,					// = 2
		ZOMBIE_DEATH				    // = 3			
	};

private:
	// ������
	explicit CNonPlayer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CNonPlayer(const CNonPlayer& _NonPlayer);
	// �Ҹ���
	virtual ~CNonPlayer() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto(CStaticMeshObject* _pStaticMesh);
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

public:
	_vec3 GetPosition(){
		return m_PartWorldMatrx[PARENT].m[3];
	}
public:
	void SetStaticMeshMap(CStaticMeshObject* _pStaticMesh){
		m_pStaticMeshMap = _pStaticMesh;
	}
	HRESULT SetExplosionDead(_vec3* vDir);
	_bool CheckHit(const _vec3& _vRayDir, const _vec3& _vRayPos, _float _fDamage);

	/* ���� ���� ��� */
private:
	// ������ ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// ����ü ������Ʈ
	CFrustum* m_pFrustumCom = nullptr;
	// �� ������Ʈ
	CStaticMeshObject* m_pStaticMeshMap = nullptr;

	/* ���� ���� ������Ʈ */
private:
	// ���̳��� �޽� ������Ʈ
	CDynamicMesh* m_pDynamicMeshCom = nullptr;
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;
	// ��ŷ ��� ������Ʈ
	CPickee* m_pPickeeCom = nullptr;
	// �׺���̼� ������Ʈ
	CNavigation* m_pNavigationCom = nullptr;

private:
	ANIMATION_SET m_eCurrAnimation = ANIMATION_SET::ZOMBIE_IDLE;

	_bool m_bIsDead = false;
	_float m_fHP = c_fZombieFullHP;
	// ������ �ð��� �帥 ������ ����
	_float m_fDeadTime = 0.f;

	// �ݶ��̴��� ������ �� ������ ���� ��Ʈ����
	_matrix m_PartWorldMatrx[PART_END];
	// �� �κ��� ��ȯ ���
	_matrix* m_pPartCombinedMatrix[PART_END];
	// �� ������ �ݶ��̴� ������Ʈ
	CCollider* m_pPartColliderCom[PART_END];
	// ���� ������ ��ø ������ ������ ����
	_bool m_bAttackCheck = false;
	// ���� ȿ���� ����ϱ� ���� ����Ʈ ������
	CEffect* m_pBloodEffect = nullptr;

private:
	// ������ ��İ� �ݶ��̴� ����
	HRESULT SettingPart();
	// ������ �ݶ��̴� ������Ʈ
	HRESULT UpdatePart();
	//������ �ݶ��̴� ����
	HRESULT RenderPart();
	// ���̴��� �������� ����
	HRESULT SettingShader();
	// ĳ������ ���� ���� ����
	void CheckState();
	// �״� ���·� ����
	void SetDeath();
	// �ִϸ��̼� ������ ���� �Լ�
	void ChangeAnimation(ANIMATION_SET _eAniSet);
	// �̵�
	HRESULT CharacterMove(_double _dDeltaTime);
	// ����
	HRESULT Attack();

public:
	// ���� ���� �Լ�(static)
	static CNonPlayer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CStaticMeshObject* _pStaticMesh = nullptr);
	// ���� ���� �Լ�
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free() override;
};

END

#endif // !__NON_PLAYER_H__
