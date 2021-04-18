#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GameObject.h"
#include "..\Header\Transform.h"
#include "..\Header\Pipeline.h"

BEGIN(Engine)
class CTransform;

class ENGINE_DLL CCamera abstract:
	public CGameObject{
public:
	typedef struct tagCameraDesc{
		// ī�޶� ���� ���� ����
		_vec3 vEye;		// ī�޶� ��ġ
		_vec3 vAt;		// ī�޶� ���� ��
		_vec3 vAxisY;	// ���� ���� Y�� ����
		// ������� ���� ����
		_float fFovY;	// �þ߰�
		_float fAspect;	// ȭ���
		_float fNear;	// ������ �ּ� �Ÿ�
		_float fFar;	// ������ �ִ� �Ÿ�
		// TRANSFORM_DESC
		CTransform::TRANSFORM_DESC tTransformDesc;
	}CAMERA_DESC;
protected:
	// ������
	explicit CCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CCamera(const CCamera& _Camera);
	// �Ҹ���
	virtual ~CCamera() = default;

public:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject();

protected:
	// Transform ������Ʈ(���� ��ȯ ���)
	CTransform* m_pTransformCom = nullptr;
	// CPipeline ����
	CPipeline* m_pPipeline = nullptr;

protected:
	CAMERA_DESC m_tCameraDesc;
	_matrix m_tViewMatrix;
	_matrix m_tProjectionMatrix;



public:
	// ���� ���� �Լ�
	virtual CGameObject * CreateClone(void* _pArgument) override PURE;
public:
	// Free
	virtual void Free();
};
END

#endif // !__CAMERA_H__
