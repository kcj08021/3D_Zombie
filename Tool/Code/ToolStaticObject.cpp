#include "stdafx.h"
#include "..\Header\ToolStaticObject.h"

USING(Tool)

CToolStaticObject::CToolStaticObject(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pStaticMeshCom(nullptr)
	, m_pPickeeCom(nullptr){
	::ZeroMemory(&m_tSettingInfo, sizeof(m_tSettingInfo));
}

CToolStaticObject::CToolStaticObject(const CToolStaticObject & _ToolStaticObject)
	: CGameObject(_ToolStaticObject)
	, m_pRendererCom(_ToolStaticObject.m_pRendererCom)
	, m_pTransformCom(_ToolStaticObject.m_pTransformCom)
	, m_pShaderCom(_ToolStaticObject.m_pShaderCom)
	, m_pStaticMeshCom(_ToolStaticObject.m_pStaticMeshCom)
	, m_pPickeeCom(_ToolStaticObject.m_pPickeeCom){
	::ZeroMemory(&m_tSettingInfo, sizeof(m_tSettingInfo));
	SafeAddRef(m_pRendererCom);
	SafeAddRef(m_pTransformCom);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pStaticMeshCom);
	SafeAddRef(m_pPickeeCom);
}


HRESULT CToolStaticObject::InitGameObject_Proto(){
	return NOERROR;
}

HRESULT CToolStaticObject::InitGameObject_Clone(void * _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	STATIC_MESH_DESC* tDesc = reinterpret_cast<STATIC_MESH_DESC*>(_pArgument);

	m_tSettingInfo = tDesc->tSettingInfo;
	lstrcpy(m_tSettingInfo.szProto, tDesc->tSettingInfo.szProto);

	_matrix matScale, matRotateX, matRotateY, matRotateZ, matTrans, matWorld;
	D3DXMatrixScaling(&matScale, tDesc->tSettingInfo.fScaleX, tDesc->tSettingInfo.fScaleY, tDesc->tSettingInfo.fScaleZ);
	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(tDesc->tSettingInfo.fRotateX));
	D3DXMatrixRotationY(&matRotateY, D3DXToRadian(tDesc->tSettingInfo.fRotateY));
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(tDesc->tSettingInfo.fRotateZ));
	D3DXMatrixTranslation(&matTrans, tDesc->tSettingInfo.fPosX, tDesc->tSettingInfo.fPosY, tDesc->tSettingInfo.fPosZ);

	matWorld = matScale * matRotateX * matRotateY * matRotateZ * matTrans;

	CTransform::TRANSFORM_DESC TransformDesc;
	TransformDesc.fRotationPerSec = 0.f;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.pMatrixWorldSetting = &matWorld;

	// 렌더러 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"DefaultShaderCom", L"DefaultShaderCom_Proto", 0)))
		return E_FAIL;

	// 트랜스폼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &(TransformDesc))))
		return E_FAIL;

	// 스태틱 메쉬 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pStaticMeshCom), L"StaticMeshCom", tDesc->tSettingInfo.szProto, 0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolStaticObject::LateInitGameObject(){
	return NOERROR;
}

HRESULT CToolStaticObject::UpdateGameObject(_double _dDeltaTime){
	if(!m_bIsLateInit)
		CGameObject::LateInitGameObject();

	return NOERROR;
}

HRESULT CToolStaticObject::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	m_pRendererCom->AddRenderList(CRenderer::RENDER_NONALPHA, this);
	
	return NOERROR;
}

HRESULT CToolStaticObject::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;

	SafeAddRef(pManagement);

	HRESULT hr = 0;

	// 월드 변환 행렬(항등행렬로 초기화)
	_matrix matWorld, matView, matProj;
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 셰이더에 객체의 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	// 카메라의 위치
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// 스태틱 메쉬의 서브셋 갯수 얻어옴
	_ulong		dwNumSubset = m_pStaticMeshCom->GetSubsetCount();

	// 셰이더를 통하여 렌더 시작
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// 서브셋 갯수만큼 반복하여 렌더
	for(_ulong i = 0; i < dwNumSubset; ++i){
		// 메쉬의 디퓨즈 텍스쳐를 셰이더에 전달
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// 노말맵이 없으면 노말매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// 스펙큘러맵이 없으면 스펙큘러매핑 끔
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}

		// Begin 이후 셰이더의 전역변수의 값이 바뀐다면 CommitChange()함수 호출
		m_pShaderCom->CommitChange();

		// 메쉬 렌더
		m_pStaticMeshCom->RenderMesh(i);
	}

	// 셰이더를 통한 렌더 종료
	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CToolStaticObject::SetInfo(OBJECT_SETTING_INFO & _Info){
	_matrix matScale, matRotateX, matRotateY, matRotateZ, matTrans, matWorld;
	D3DXMatrixScaling(&matScale, _Info.fScaleX, _Info.fScaleY, _Info.fScaleZ);
	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(_Info.fRotateX));
	D3DXMatrixRotationY(&matRotateY, D3DXToRadian(_Info.fRotateY));
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(_Info.fRotateZ));
	D3DXMatrixTranslation(&matTrans, _Info.fPosX, _Info.fPosY, _Info.fPosZ);

	matWorld = matScale * matRotateX * matRotateY * matRotateZ * matTrans;

	m_pTransformCom->SetWorldMatrix(matWorld);

	m_tSettingInfo = _Info;

	return NOERROR;
}

CToolStaticObject * CToolStaticObject::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CToolStaticObject* pInstance = new CToolStaticObject(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CToolStaticObject Create Failure");
	}
	
	return pInstance;
}

CGameObject * CToolStaticObject::CreateClone(void * _pArgument){
	CGameObject* pInstance = new CToolStaticObject(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CToolStaticObject Clone Failure");
	}

	return pInstance;
}

void CToolStaticObject::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pStaticMeshCom);
	SafeRelease(m_pPickeeCom);
	SafeDelete(m_szLayer);

	CGameObject::Free();
}
