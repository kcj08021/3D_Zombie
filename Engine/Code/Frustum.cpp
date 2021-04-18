#include "..\Header\Frustum.h"
#include "..\Header\Pipeline.h"
#include "..\Header\Transform.h"


CFrustum::CFrustum(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pPipeline(CPipeline::GetInstance()){
	SafeAddRef(m_pPipeline);
}

HRESULT CFrustum::InitComponent_Proto(){
	m_vProjPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_vProjPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_vProjPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_vProjPoint[3] = _vec3(-1.f, -1.f, 0.f);

	m_vProjPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_vProjPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_vProjPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_vProjPoint[7] = _vec3(-1.f, -1.f, 1.f);

	return NOERROR;
}

HRESULT CFrustum::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

_bool CFrustum::CheckFrustum(const _matrix* _pWorldMatrix, _float _fRadius/* = 0.f*/){
	_vec3 vLocalPos = {};

	if(FAILED(SettingFrustum(_pWorldMatrix, &vLocalPos)))
		return false;

	for(_uint i = 0; i < 6; ++i){
		if(D3DXPlaneDotCoord(&m_Plane[i], &vLocalPos) > _fRadius)
			return false;
	}

	return true;
}

HRESULT CFrustum::SettingFrustum(const _matrix* _pWorldMatrix, _vec3* _pLocalPos){
	if(nullptr == m_pPipeline
		|| nullptr == _pWorldMatrix)
		return E_FAIL;

	_vec3 vPoint[8];
	_matrix matWorldInv, matViewInv, matProjInv;

	D3DXMatrixInverse(&matWorldInv, nullptr, _pWorldMatrix);

	m_pPipeline->GetTransform(D3DTS_VIEW, &matViewInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matViewInv);

	m_pPipeline->GetTransform(D3DTS_PROJECTION, &matProjInv);
	D3DXMatrixInverse(&matProjInv, nullptr, &matProjInv);

	for(_uint i = 0; i < 8; ++i){
		D3DXVec3TransformCoord(&vPoint[i], &m_vProjPoint[i], &matProjInv);

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matViewInv);

		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matWorldInv);
	}

	D3DXPlaneFromPoints(&m_Plane[0], &vPoint[1], &vPoint[5], &vPoint[6]);
	D3DXPlaneFromPoints(&m_Plane[1], &vPoint[4], &vPoint[0], &vPoint[3]);
	D3DXPlaneFromPoints(&m_Plane[2], &vPoint[4], &vPoint[5], &vPoint[1]);

	D3DXPlaneFromPoints(&m_Plane[3], &vPoint[3], &vPoint[2], &vPoint[6]);
	D3DXPlaneFromPoints(&m_Plane[4], &vPoint[5], &vPoint[4], &vPoint[7]);
	D3DXPlaneFromPoints(&m_Plane[5], &vPoint[0], &vPoint[1], &vPoint[2]);

	D3DXVec3TransformCoord(_pLocalPos, &_vec3(_pWorldMatrix->m[3]), &matWorldInv);

	return NOERROR;
}

CFrustum * CFrustum::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CFrustum* pInstance = new CFrustum(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CFrustum Create Failure");
	}
	return pInstance;
}

CComponent * CFrustum::CreateClone(void * _pArgument){
	AddRef();

	return this;
}

void CFrustum::Free(){
	SafeRelease(m_pPipeline);

	CComponent::Free();
}
