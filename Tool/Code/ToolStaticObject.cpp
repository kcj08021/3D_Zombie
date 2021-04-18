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

	// ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	// ���̴� ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"DefaultShaderCom", L"DefaultShaderCom_Proto", 0)))
		return E_FAIL;

	// Ʈ������ ������Ʈ �߰�
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &(TransformDesc))))
		return E_FAIL;

	// ����ƽ �޽� ������Ʈ �߰�
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

	// ���� ��ȯ ���(�׵���ķ� �ʱ�ȭ)
	_matrix matWorld, matView, matProj;
	matWorld = m_pTransformCom->GetWorldMatrix();
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// ���̴��� ��ü�� ����, ��, ���� ��� ����
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	// ī�޶��� ������� ������
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);

	// ī�޶��� ��ġ
	m_pShaderCom->SetValue("g_vCameraPosition", matCameraWorld.m[3], sizeof(_vec4));

	// ����ƽ �޽��� ����� ���� ����
	_ulong		dwNumSubset = m_pStaticMeshCom->GetSubsetCount();

	// ���̴��� ���Ͽ� ���� ����
	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	// ����� ������ŭ �ݺ��Ͽ� ����
	for(_ulong i = 0; i < dwNumSubset; ++i){
		// �޽��� ��ǻ�� �ؽ��ĸ� ���̴��� ����
		LPDIRECT3DTEXTURE9 Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_DIFFUSE);
		m_pShaderCom->SetTexture("g_DiffuseTexture", Texture);

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_NORMAL);
		// �븻���� ������ �븻���� ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsNormalMapping", true);
			m_pShaderCom->SetTexture("g_NormalTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsNormalMapping", false);
		}

		Texture = m_pStaticMeshCom->GetMaterialTexture(i, MESH_TEXTURE::TYPE_SPECULAR);
		// ����ŧ������ ������ ����ŧ������ ��
		if(nullptr != Texture){
			m_pShaderCom->SetBool("g_bIsSpecularMapping", true);
			m_pShaderCom->SetTexture("g_SpecularTexture", Texture);
		} else{
			m_pShaderCom->SetBool("g_bIsSpecularMapping", false);
		}

		// Begin ���� ���̴��� ���������� ���� �ٲ�ٸ� CommitChange()�Լ� ȣ��
		m_pShaderCom->CommitChange();

		// �޽� ����
		m_pStaticMeshCom->RenderMesh(i);
	}

	// ���̴��� ���� ���� ����
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
