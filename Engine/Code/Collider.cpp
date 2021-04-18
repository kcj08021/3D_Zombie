#include "..\Header\Collider.h"
#include "..\Header\Shader.h"
#include "..\Header\Transform.h"
#include "..\Header\Pipeline.h"
#include "..\Header\CollisionManager.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pMesh(nullptr)
	, m_pAdjacency(nullptr)
	, m_pColliderShader(nullptr)
	, m_pPipeline(nullptr)
	, m_bIsCollision(false)
	, m_bIsEnable(false)
	, m_pOBB_Desc(nullptr){
	::ZeroMemory(&m_tWorldMatrix, sizeof(_matrix));
	::ZeroMemory(&m_vMin_AABB, sizeof(_vec3));
	::ZeroMemory(&m_vMax_AABB, sizeof(_vec3));
}

CCollider::CCollider(const CCollider & _Collider)
	: CComponent(_Collider)
	, m_pColliderShader(_Collider.m_pColliderShader)
	, m_pPipeline(_Collider.m_pPipeline)
	, m_eType(_Collider.m_eType)
	, m_pMesh(nullptr)
	, m_tWorldMatrix(_Collider.m_tWorldMatrix)
	, m_vMin_AABB(_Collider.m_vMin_AABB)
	, m_vMax_AABB(_Collider.m_vMax_AABB)
	, m_pOBB_Desc(_Collider.m_pOBB_Desc)
	, m_bIsCollision(false)
	, m_bIsEnable(false){
	SafeAddRef(m_pColliderShader);
	SafeAddRef(m_pPipeline);

	_Collider.m_pMesh->CloneMeshFVF(_Collider.m_pMesh->GetOptions(), _Collider.m_pMesh->GetFVF(), CComponent::GetGraphicDevice(), &m_pMesh);
}

HRESULT CCollider::InitComponent_Proto(COLL_TYPE _eType){
	m_eType = _eType;
	
	switch(m_eType){
	case Engine::CCollider::COLLTYPE_AABB:
	case Engine::CCollider::COLLTYPE_OBB:
		if(FAILED(D3DXCreateBox(CComponent::GetGraphicDevice(), 1.f, 1.f, 1.f, &m_pMesh, &m_pAdjacency)))
			return E_FAIL;
		break;
	case Engine::CCollider::COLLTYPE_SPHERE:
		if(FAILED(D3DXCreateSphere(CComponent::GetGraphicDevice(), 0.5f, 10, 10, &m_pMesh, &m_pAdjacency)))
			return E_FAIL;
		break;
	default:
		break;
	}

	// �ݶ��̴������� ���Ǵ� ���̴�, �������� �����, �ݶ��̴������� ����� �����ϴ�.
	m_pColliderShader = CShader::CreateProto(CComponent::GetGraphicDevice(), L"../Bin/ShaderFile/ColliderShader.fx");
	if(nullptr == m_pColliderShader)
		return E_FAIL;

	m_pPipeline = CPipeline::GetInstance();
	if(nullptr == m_pPipeline)
		return E_FAIL;
	SafeAddRef(m_pPipeline);

	// �ݶ��̴��� ��������� �̸� �׵���ķ� �������´�.
	D3DXMatrixIdentity(&m_tWorldMatrix);

	// �浹 �Ŵ����� �ּҸ� �̸� �޾Ƴ���


	return NOERROR;
}

HRESULT CCollider::InitComponent_Clone(void * _pArgument){
	if(nullptr == m_pMesh)
		return E_FAIL;

	m_tColliderDesc = *reinterpret_cast<COLLIDER_DESC*>(_pArgument);

	_matrix ScaleMatrix, PostionMatrix, LocalTransformMatrix;

	// �ݶ��̴��� ������ ����� ���
	switch(m_eType){
	case Engine::CCollider::COLLTYPE_AABB:
	case Engine::CCollider::COLLTYPE_OBB:
		D3DXMatrixScaling(&ScaleMatrix, m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.y, m_tColliderDesc.vScale.z);
		break;
	case Engine::CCollider::COLLTYPE_SPHERE:
		D3DXMatrixScaling(&ScaleMatrix, m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.x);
		break;
	default:
		break;
	}

	// �ݶ��̴��� ��ġ ����� ���
	D3DXMatrixTranslation(&PostionMatrix, m_tColliderDesc.vLocalPosition.x, m_tColliderDesc.vLocalPosition.y, m_tColliderDesc.vLocalPosition.z);
	
	// ���û󿡼� ��ȯ�� �ϱ� ���� ���
	LocalTransformMatrix = ScaleMatrix * PostionMatrix;

	// �ݶ��̴� �޽��� ��ȯ �غ�
	_ulong dwVerticesCount = m_pMesh->GetNumVertices();				// ���� ����
	_uint uiStride = D3DXGetFVFVertexSize(m_pMesh->GetFVF());		// ���� �Ѱ��� ������

	void* pVertices = nullptr;

	m_pMesh->LockVertexBuffer(0, &pVertices);

	// �޽��� ������ ��ȯ��
	for(_uint i = 0; i < dwVerticesCount; ++i)
		_vec3* vec = D3DXVec3TransformCoord(reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + i*uiStride), reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + i*uiStride), &LocalTransformMatrix);
	
	// AABB �浹 ����� ���� Min�� Max�� ���ϴ� �Լ�
	D3DXComputeBoundingBox(reinterpret_cast<_vec3*>(pVertices), dwVerticesCount, uiStride, &m_vMin_AABB, &m_vMax_AABB);

	m_pMesh->UnlockVertexBuffer();

	// �ݶ��̴��� ��������� ��ġ�� �θ��� ��ġ�� ����
	::memcpy(&m_tWorldMatrix.m[3], m_tColliderDesc.pParentWorldMatrix->m[3], sizeof(_vec3));

	// ���浹 �ݶ��̴��� �ƴ� ��� OBB ������ ����
	if(m_eType != COLLTYPE_SPHERE){
		m_pOBB_Desc = new OBB_DESC;
		
		_vec3 vPoint[8]{
			// ����ü�� �ո�(Z���� ���� �κ�)
			{ m_vMin_AABB.x, m_vMax_AABB.y, m_vMin_AABB.z },
			{ m_vMax_AABB.x, m_vMax_AABB.y, m_vMin_AABB.z },
			{ m_vMax_AABB.x, m_vMin_AABB.y, m_vMin_AABB.z },
			m_vMin_AABB,
			// ����ü�� �޸�(Z���� ū �κ�)
			{ m_vMin_AABB.x, m_vMax_AABB.y, m_vMax_AABB.z },
			m_vMax_AABB,
			{ m_vMax_AABB.x, m_vMin_AABB.y, m_vMax_AABB.z },
			{ m_vMin_AABB.x, m_vMin_AABB.y, m_vMax_AABB.z },
		};

		// OBB �߽�
		m_pOBB_Desc->vCenterPos = (vPoint[3] + vPoint[5])*0.5f;

		// ����ü �� ���п� ������ ��
		m_pOBB_Desc->vAlignAxis[0] = vPoint[2] - vPoint[3];
		m_pOBB_Desc->vAlignAxis[1] = vPoint[0] - vPoint[3];
		m_pOBB_Desc->vAlignAxis[2] = vPoint[7] - vPoint[3];

		//
		m_pOBB_Desc->vProjAxis[0] = (vPoint[5] + vPoint[2])*0.5f - m_pOBB_Desc->vCenterPos;
		m_pOBB_Desc->vProjAxis[1] = (vPoint[5] + vPoint[0])*0.5f - m_pOBB_Desc->vCenterPos;
		m_pOBB_Desc->vProjAxis[2] = (vPoint[5] + vPoint[7])*0.5f - m_pOBB_Desc->vCenterPos;
	}

	return NOERROR;
}
// �浹 �Ŵ����� �ݶ��̴� ���
HRESULT CCollider::SettingToCollisionManager(){
	CCollisionManager*  pCollisionManager = CCollisionManager::GetInstance();
	if(nullptr == pCollisionManager)
		return E_FAIL;
	SafeAddRef(pCollisionManager);

	if(FAILED(pCollisionManager->AddCollider(this))){
		SafeRelease(pCollisionManager);
		return E_FAIL;
	}

	SafeRelease(pCollisionManager);

	return NOERROR;
}

HRESULT CCollider::UpdateCollider(){
	switch(m_eType){
	case CCollider::COLLTYPE_AABB:
		::memcpy(m_tWorldMatrix.m[3], m_tColliderDesc.pParentWorldMatrix->m[3], sizeof(_vec3));
		break;
	case CCollider::COLLTYPE_OBB:
		m_tWorldMatrix = *m_tColliderDesc.pParentWorldMatrix;
		D3DXVec3Normalize(reinterpret_cast<_vec3*>(m_tWorldMatrix.m[0]), reinterpret_cast<_vec3*>(m_tWorldMatrix.m[0]));
		D3DXVec3Normalize(reinterpret_cast<_vec3*>(m_tWorldMatrix.m[1]), reinterpret_cast<_vec3*>(m_tWorldMatrix.m[1]));
		D3DXVec3Normalize(reinterpret_cast<_vec3*>(m_tWorldMatrix.m[2]), reinterpret_cast<_vec3*>(m_tWorldMatrix.m[2]));
		break;	
	case CCollider::COLLTYPE_SPHERE:
		::memcpy(m_tWorldMatrix.m[3], m_tColliderDesc.pParentWorldMatrix->m[3], sizeof(_vec3));
		break;
	default:
		break;
	}

	return NOERROR;
}

HRESULT CCollider::RenderCollider(){
	if(nullptr == m_pColliderShader
		|| nullptr == m_pPipeline
		|| nullptr == m_pMesh)
		return E_FAIL;

	_matrix tViewMatrix, tProjectionMatrix;

	m_pPipeline->GetTransform(D3DTS_VIEW, &tViewMatrix);
	m_pPipeline->GetTransform(D3DTS_PROJECTION, &tProjectionMatrix);

	m_pColliderShader->SetValue("g_matWorld", &m_tWorldMatrix, sizeof(_matrix));
	m_pColliderShader->SetValue("g_matView", &tViewMatrix, sizeof(_matrix));
	m_pColliderShader->SetValue("g_matProj", &tProjectionMatrix, sizeof(_matrix));

	m_pColliderShader->SetBool("g_bIsCollision", m_bIsCollision);

	m_pColliderShader->BeginShader();
	m_pColliderShader->BeginPass(0);
	
	m_pMesh->DrawSubset(0);

	m_pColliderShader->EndPass();
	m_pColliderShader->EndShader();

	return NOERROR;
}

HRESULT CCollider::DeleteToCollisionManager(){
	CCollisionManager*  pCollisionManager = CCollisionManager::GetInstance();
	if(nullptr == pCollisionManager)
		return false;
	SafeAddRef(pCollisionManager);

	pCollisionManager->RemoveCollider(this);

	SafeRelease(pCollisionManager);

	return NOERROR;
}

_bool CCollider::IsCollision(){
	CCollisionManager*  pCollisionManager = CCollisionManager::GetInstance();
	if(nullptr == pCollisionManager)
		return false;
	SafeAddRef(pCollisionManager);

	m_pCollisionObject = nullptr;
	m_bIsCollision = false;
	_bool bIsCollision = false;

	if(FAILED(bIsCollision = pCollisionManager->IsCollision(this))){
		SafeRelease(pCollisionManager);
		return false;
	}

	SafeRelease(pCollisionManager);

	if(m_bIsCollision != bIsCollision
		&& bIsCollision)
		m_bIsCollision = bIsCollision;

	return bIsCollision;
}

CCollider * CCollider::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, COLL_TYPE _eType){
	CCollider* pInstance = new CCollider(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_eType))){
		SafeRelease(pInstance);
		MSG_BOX(L"CCollider Create Failure");
	}
	return pInstance;
}

CComponent * CCollider::CreateClone(void * _pArgument){
	CCollider* pInstance = new CCollider(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CCollider Clone Failure");
	}
	return pInstance;
}

void CCollider::Free(){
	SafeDelete(m_pOBB_Desc);
	SafeRelease(m_pMesh);
	SafeRelease(m_pColliderShader);
	SafeRelease(m_pPipeline);

	CComponent::Free();
}
