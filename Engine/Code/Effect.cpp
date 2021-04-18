#include "..\Header\Effect.h"
#include "..\Header\Management.h"


CEffect::CEffect(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRendererCom(nullptr)
	, m_pVIBufferCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_uiShaderPassNum(0)
	, m_pTextureCom(nullptr)
	, m_uiTextureCount(0)
	, m_pTransformCom(nullptr)
	, m_bIsEffectOn(false)
	, m_uiEffectPoolSize(0)
	, m_uiCurrIndex(0)
	, m_fMaxFrame(0.f), m_fPlayTime(0.f){
}

CEffect::CEffect(const CEffect & _Effect)
	: CGameObject(_Effect)
	, m_pRendererCom(_Effect.m_pRendererCom)
	, m_pVIBufferCom(_Effect.m_pVIBufferCom)
	, m_pShaderCom(nullptr)
	, m_uiShaderPassNum(0)
	, m_pTextureCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_uiTextureCount(0)
	, m_bIsEffectOn(false)
	, m_uiEffectPoolSize(0)
	, m_uiCurrIndex(0)
	, m_fMaxFrame(0.f), m_fPlayTime(0.f){
	SafeAddRef(m_pRendererCom);
	SafeAddRef(m_pShaderCom);
	SafeAddRef(m_pVIBufferCom);
	SafeAddRef(m_pTextureCom);
	SafeAddRef(m_pTransformCom);
}

HRESULT CEffect::InitGameObject_Proto(){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pVIBufferCom), L"VIBufferCom", L"VIBufferRectCom_Proto", 0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect::InitGameObject_Clone(void * _pArgument){
	if(nullptr == _pArgument)
		return E_FAIL;

	EFFECT_DESC* pDesc = reinterpret_cast<EFFECT_DESC*>(_pArgument);

	m_eType = pDesc->eType;

	m_EffectPool.resize(m_uiEffectPoolSize=pDesc->uiEffectPoolCount);

	m_fPlayTime = pDesc->fEffectPlayTime;

	CTransform::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.fRotationPerSec = 0;
	TransformDesc.fSpeedPerSec = 0;
	TransformDesc.pMatrixWorldSetting = pDesc->pWorldMatrix;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &TransformDesc)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"EffectShaderCom", pDesc->szShaderFileName != nullptr ? pDesc->szShaderFileName : L"EffectShader_Proto", pDesc->szShaderFileName != nullptr ? pDesc->uiSceneNumber : 0)))
		return E_FAIL;
	m_uiShaderPassNum = pDesc->szShaderPassNum;

	m_uiTextureCount = pDesc->uiTextureCount;
	m_fMaxFrame = pDesc->fMaxFrame;

	// 텍스쳐 컴포넌트 추가
	if(nullptr != pDesc->szTextureName)
		if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTextureCom), L"TextureCom", pDesc->szTextureName, pDesc->uiSceneNumber)))
			return E_FAIL;

	return NOERROR;
}

HRESULT CEffect::LateInitGameObject(){
	return NOERROR;
}

HRESULT CEffect::UpdateGameObject(_double _dDeltaTime){
	for(auto &elem : m_EffectPool){
		if(m_eType && elem.bIsEffectOn){
			elem.fFrame += m_fMaxFrame * static_cast<_float>(_dDeltaTime) * (1 / m_fPlayTime);

			if(elem.fFrame >= m_fMaxFrame){
				elem.fFrame = 0.f;
				elem.bIsAnimationEnd = true;
			}
		}
	}
	SettingBillboard();

	return NOERROR;
}

HRESULT CEffect::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRendererCom)
		return E_FAIL;

	for(auto& elem : m_EffectPool){
		if(elem.bIsAnimationEnd == true){
			elem.bIsEffectOn = false;
		}
		if(elem.bIsEffectOn){
			m_bIsEffectOn = true;
			break;
		}
	}

	if(m_bIsEffectOn){
		CGameObject::ComputeViewZ(m_pTransformCom->GetState(CTransform::STATE_POSITION));
		m_pRendererCom->AddRenderList(CRenderer::RENDER_ALPHA, this);
	}

	return NOERROR;
}

HRESULT CEffect::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pVIBufferCom)
		return E_FAIL;


	_matrix matView;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr != pManagement){
		SafeAddRef(pManagement);

		D3DXMatrixIdentity(&matView);

		pManagement->GetTransform(D3DTS_VIEW, &matView);

		SafeRelease(pManagement);
	}


	list<EFFECT_STRUCT*> listSort;
	for(auto& elem : m_EffectPool){
		if(elem.bIsEffectOn){
			_vec3 vViewPos;
			D3DXVec3TransformCoord(&vViewPos, &(elem.vPos), &matView);
			elem.fViewZ = vViewPos.z;
			listSort.push_back(&elem);
		}
	}

	if(listSort.size() > 1)
		listSort.sort([](EFFECT_STRUCT* _pSrc, EFFECT_STRUCT* _pDest)->_bool{
		return _pSrc->fViewZ > _pDest->fViewZ;
	});

	for(auto elem : listSort){
		if(!elem->bIsEffectOn)
			continue;

		if(FAILED(SettingShader(elem)))
			return E_FAIL;

		m_pShaderCom->BeginShader();
		m_pShaderCom->BeginPass(m_uiShaderPassNum);

		m_pVIBufferCom->RenderVIBufferRect();

		m_pShaderCom->EndPass();
		m_pShaderCom->EndShader();
	}

	return NOERROR;
}

HRESULT CEffect::SetEffectPosition(_vec3 _vPos){
	if(nullptr == m_pTransformCom)
		return E_FAIL;

	if(1 == m_uiEffectPoolSize){
		m_pTransformCom->SetState(CTransform::STATE_POSITION, _vPos);
	}
	else{
		if(m_uiCurrIndex == m_uiEffectPoolSize)
			m_uiCurrIndex = 0;

		m_EffectPool[m_uiCurrIndex].vPos = _vPos;
		m_EffectPool[m_uiCurrIndex].bIsEffectOn = true;
		m_EffectPool[m_uiCurrIndex].bIsAnimationEnd = false;
		m_EffectPool[m_uiCurrIndex++].fFrame = 0.f;
	}

	return NOERROR;
}

void CEffect::SetEffectStatus(_bool _IsEffectOn, _uint _uiIndex /* = 0*/){
	m_EffectPool[_uiIndex].bIsEffectOn = _IsEffectOn;
	m_EffectPool[_uiIndex].bIsAnimationEnd = false;
}

HRESULT CEffect::SettingBillboard(){
	if(nullptr == m_pTransformCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	_matrix matView;
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_matrix matWorld = m_pTransformCom->GetWorldMatrix();
	m_pTransformCom->SetState(CTransform::STATE_RIGHT, _vec3(matView.m[0])* D3DXVec3Length(reinterpret_cast<_vec3*>(matWorld.m[0])));
	m_pTransformCom->SetState(CTransform::STATE_UP, _vec3(matView.m[1])* D3DXVec3Length(reinterpret_cast<_vec3*>(matWorld.m[1])));
	m_pTransformCom->SetState(CTransform::STATE_LOOK, _vec3(matView.m[2])* D3DXVec3Length(reinterpret_cast<_vec3*>(matWorld.m[2])));

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CEffect::SettingShader(const EFFECT_STRUCT* _EffectStruct){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pTransformCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	if(1 != m_uiEffectPoolSize)
		m_pTransformCom->SetState(CTransform::STATE_TYPE::STATE_POSITION, _EffectStruct->vPos);

	_matrix matView, matProj;

	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pShaderCom->SetValue("g_matWorld", m_pTransformCom->GetWorldMatrixPointer(), sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	_float fFrame = _EffectStruct->fFrame;

	switch(m_eType){
	case Engine::CEffect::SIMPLE:
	case Engine::CEffect::ANIMATION:
		if(m_uiTextureCount > 1)
			m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture(static_cast<_uint>(fFrame)));
		else{
			m_pShaderCom->SetValue("g_fFrame", &fFrame, sizeof(_float));
			m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture());
		}
		break;
	case Engine::CEffect::RANDOM:
		m_pShaderCom->SetTexture("g_DiffuseTexture", m_pTextureCom->GetTexture(rand() % 4));
		break;
	default:
		break;
	}

	pManagement->SetttingRenderTargetOnShader(m_pShaderCom, "g_DepthTexture", L"DepthTarget");

	SafeRelease(pManagement);

	return NOERROR;
}

void CEffect::Free(){
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pVIBufferCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pTransformCom);


	CGameObject::Free();
}
