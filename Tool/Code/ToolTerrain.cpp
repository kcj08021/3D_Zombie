#include "stdafx.h"
#include "..\Header\ToolTerrain.h"

USING(Tool)

CToolTerrain::CToolTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pFilterTextureCom(nullptr)
	, m_pBrushTextureCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pVIBufferTerrainCom(nullptr)
	, m_pPickeeCom(nullptr)
	, m_uiShaderPass(0)
	, m_uiHeightValue(0)
	, m_uiOuterCircleSize(0)
	, m_uiInnerCircleSize(0)
	, m_bIsCursorRect(false){

}

CToolTerrain::CToolTerrain(const CToolTerrain & _ToolTerrain)
	: CGameObject(_ToolTerrain)
	, m_pShaderCom(_ToolTerrain.m_pShaderCom)
	, m_pTextureCom(_ToolTerrain.m_pTextureCom)
	, m_pFilterTextureCom(_ToolTerrain.m_pFilterTextureCom)
	, m_pBrushTextureCom(_ToolTerrain.m_pBrushTextureCom)
	, m_pRendererCom(_ToolTerrain.m_pRendererCom)
	, m_pTransformCom(_ToolTerrain.m_pTransformCom)
	, m_pVIBufferTerrainCom(_ToolTerrain.m_pVIBufferTerrainCom)
	, m_pPickeeCom(_ToolTerrain.m_pPickeeCom)
	, m_uiShaderPass(_ToolTerrain.m_uiShaderPass)
	, m_uiHeightValue(_ToolTerrain.m_uiHeightValue)
	, m_uiOuterCircleSize(_ToolTerrain.m_uiOuterCircleSize)
	, m_uiInnerCircleSize(_ToolTerrain.m_uiInnerCircleSize)
	, m_bIsCursorRect(_ToolTerrain.m_bIsCursorRect){
}

// Initialize with Prototype
HRESULT CToolTerrain::InitGameObject_Proto(){
	SettingCloneComponent();

	return NOERROR;
}

// Initialize with Clone
HRESULT CToolTerrain::InitGameObject_Clone(void * _pArgument){
	return NOERROR;
}

// LateInitialize
HRESULT CToolTerrain::LateInitGameObject(){
	return NOERROR;
}

// Update
HRESULT CToolTerrain::UpdateGameObject(_double _dDeltaTime){
	return NOERROR;
}

// LateUpdate
HRESULT CToolTerrain::LateUpdateGameObject(_double _dDeltaTime){
	if(FAILED(m_pRendererCom->AddRenderList(CRenderer::RENDER_PRIORITY, this)))
		return E_FAIL;

	return NOERROR;
}

// Render
HRESULT CToolTerrain::RenderGameObject(_double _dDeltaTime){
	if(nullptr == m_pShaderCom
		|| nullptr == m_pVIBufferTerrainCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	_matrix matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	// 카메라의 월드행렬 가져옴
	_matrix matCameraWorld = {};
	pManagement->GetCameraWorldMatrix(&matCameraWorld);


	// 셰이더에 월드, 뷰, 투영 행렬 전달
	m_pShaderCom->SetValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(m_uiShaderPass);

	m_pVIBufferTerrainCom->RenderVIBufferTerrain();

	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);

	return NOERROR;
}


// 지형 버퍼를 받아와 설정함
HRESULT CToolTerrain::SettingVIBufferTerrain(){
	if(nullptr != m_pVIBufferTerrainCom){
		if(FAILED(ReleaseComponent(reinterpret_cast<CComponent**>(&m_pVIBufferTerrainCom), L"VIBufferTerrainCom")))
			return E_FAIL;
	}
	
	if(FAILED(AddComponent(reinterpret_cast<CComponent**>(&m_pVIBufferTerrainCom), L"VIBufferTerrainCom", L"VIBufferTerrain_Proto", 0)))
		return E_FAIL;

	CPickee::PICKEE_DESC PickeeDesc;


	PickeeDesc.pGameObj = this;
	m_pVIBufferTerrainCom->GetPickingCheckInfo(PickeeDesc.pPolygonPosition, PickeeDesc.uiPrimitiveCount);
	PickeeDesc.pWolrdMatrix = m_pTransformCom->GetWorldMatrixPointer();
	m_pPickeeCom->ChangePickeeDesc(&PickeeDesc);

	return NOERROR;
}

// 지정된 높이와 범위로 지형 편집
HRESULT CToolTerrain::EditHeight(const _vec3 & _vPos , _double _dDeltaTime, _bool _bIsContinuallyMode){
	if(nullptr == m_pVIBufferTerrainCom
		|| nullptr == m_pPickeeCom)
		return E_FAIL;

	if(!m_bIsCursorRect){
		if(FAILED(EditHeightToCircle(_vPos, _dDeltaTime, _bIsContinuallyMode)))
			return E_FAIL;
	} else{
		if(FAILED(EditHeightToRect(_vPos, _dDeltaTime, _bIsContinuallyMode)))
			return E_FAIL;
	}

	// 피킹 정보 업데이트
	CPickee::PICKEE_DESC PickeeDesc;

	PickeeDesc.pGameObj = this;
	m_pVIBufferTerrainCom->GetPickingCheckInfo(PickeeDesc.pPolygonPosition, PickeeDesc.uiPrimitiveCount);
	PickeeDesc.pWolrdMatrix = m_pTransformCom->GetWorldMatrixPointer();
	
	m_pPickeeCom->ChangePickeeDesc(&PickeeDesc);

	
	return NOERROR;
}

// 높이맵을 저장
HRESULT CToolTerrain::SaveHeightMap(const _tchar * _szPath){
	if(nullptr == m_pVIBufferTerrainCom){
		return E_FAIL;
	}

	LPDIRECT3DDEVICE9 pGraphicDevice = CGameObject::GetGraphicDevice();
	if(nullptr == pGraphicDevice)
		return E_FAIL;

	LPDIRECT3DTEXTURE9 pTexture = nullptr;
	
	if(FAILED(m_pVIBufferTerrainCom->GetHeightMap(&pTexture, 50.f)))
		return E_FAIL;
	
	D3DXSaveTextureToFile(_szPath, D3DXIFF_BMP, pTexture, nullptr);

	SafeRelease(pTexture);

	return NOERROR;
}

// 높이맵을 불러오기
HRESULT CToolTerrain::LoadHeightMap(const _tchar * _szPath){
	return E_NOTIMPL;
}

// 텍스쳐를 받아와 설정함
HRESULT CToolTerrain::SetTerrainTexture(CTexture * _pTextureCom){
	if(nullptr == _pTextureCom)
		return E_FAIL;

	if(nullptr != m_pTextureCom)
		SafeRelease(m_pTextureCom);

	m_pTextureCom = _pTextureCom;

	return NOERROR;
}

HRESULT CToolTerrain::SettingCloneComponent(){

	// 렌더러 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRendererCom), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc = { nullptr , 0.f,0.f };

	// 트랜스폼 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pTransformCom), L"TransformCom", L"TransformCom_Proto", 0, &TransformDesc)))
		return E_FAIL;

	// 셰이더 컴포넌트 추가
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"TerrainShaderCom", L"TerrainShaderCom_Proto", 0)))
		return E_FAIL;

	CPickee::PICKEE_DESC PickeeDesc;

	PickeeDesc.pGameObj = this;
	PickeeDesc.pPolygonPosition = nullptr;
	PickeeDesc.uiPrimitiveCount = 0;
	PickeeDesc.pWolrdMatrix = m_pTransformCom->GetWorldMatrixPointer();

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pPickeeCom), L"PickeeCom", L"PickeeCom_Proto", 0, &PickeeDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolTerrain::EditHeightToCircle(const _vec3 & _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode){
	const POLYGON_POSITION* pPolygonSrcPosition = nullptr;
	_uint uiPrimitiveCount = 0;
	m_pVIBufferTerrainCom->GetPickingCheckInfo(pPolygonSrcPosition, uiPrimitiveCount);

	POLYGON_POSITION* pPolygonDestPosition = new POLYGON_POSITION[uiPrimitiveCount];
	::memcpy(pPolygonDestPosition, pPolygonSrcPosition, sizeof(POLYGON_POSITION)* uiPrimitiveCount);

	for(_uint i = 0; i < uiPrimitiveCount; ++i){
		_vec3 vDistance1 = _vec3{ pPolygonDestPosition[i]._1.x - _vPos.x, 0.f, pPolygonDestPosition[i]._1.z - _vPos.z };
		_vec3 vDistance2 = _vec3{ pPolygonDestPosition[i]._2.x - _vPos.x, 0.f, pPolygonDestPosition[i]._2.z - _vPos.z };
		_vec3 vDistance3 = _vec3{ pPolygonDestPosition[i]._3.x - _vPos.x, 0.f, pPolygonDestPosition[i]._3.z - _vPos.z };

		float fLength1 = D3DXVec3Length(&vDistance1);
		float fLength2 = D3DXVec3Length(&vDistance2);
		float fLength3 = D3DXVec3Length(&vDistance3);
		if(_bIsContinuallyMode){
			float fSrcHeight = (0 == m_uiHeightValue) ? -1.f : m_uiHeightValue;

			if(fLength1 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._1.y += static_cast<float>(fSrcHeight * _dDeltaTime);
			else if(fLength1 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength1 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength1 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * fSrcHeight;
				pPolygonDestPosition[i]._1.y += static_cast<float>(fHeight * _dDeltaTime);
			}

			if(fLength2 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._2.y += static_cast<float>(fSrcHeight * _dDeltaTime);
			else if(fLength2 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength2 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength2 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * fSrcHeight;
				pPolygonDestPosition[i]._2.y = static_cast<float>(fHeight * _dDeltaTime);
			}

			if(fLength3 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._3.y += static_cast<float>(fSrcHeight * _dDeltaTime);
			else if(fLength3 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength3 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength3 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * fSrcHeight;
				pPolygonDestPosition[i]._3.y += static_cast<float>(fHeight * _dDeltaTime);
			}

			// 0 이상으로 제한
			pPolygonDestPosition[i]._1.y = (0.f > pPolygonDestPosition[i]._1.y) ? 0.f : pPolygonDestPosition[i]._1.y;
			pPolygonDestPosition[i]._2.y = (0.f > pPolygonDestPosition[i]._2.y) ? 0.f : pPolygonDestPosition[i]._2.y;
			pPolygonDestPosition[i]._3.y = (0.f > pPolygonDestPosition[i]._3.y) ? 0.f : pPolygonDestPosition[i]._3.y;

			// 50 이하로 제한
			pPolygonDestPosition[i]._1.y = (50 < pPolygonDestPosition[i]._1.y) ? 50.f : pPolygonDestPosition[i]._1.y;
			pPolygonDestPosition[i]._2.y = (50.f < pPolygonDestPosition[i]._2.y) ? 50.f : pPolygonDestPosition[i]._2.y;
			pPolygonDestPosition[i]._3.y = (50.f < pPolygonDestPosition[i]._3.y) ? 50.f : pPolygonDestPosition[i]._3.y;
		} else{
			if(fLength1 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._1.y = static_cast<float>(m_uiHeightValue);
			else if(fLength1 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength1 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength1 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * m_uiHeightValue;
				pPolygonDestPosition[i]._1.y = pPolygonDestPosition[i]._1.y < fHeight ? fHeight : pPolygonDestPosition[i]._1.y;
			}

			if(fLength2 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._2.y = static_cast<float>(m_uiHeightValue);
			else if(fLength2 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength2 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength2 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * m_uiHeightValue;
				pPolygonDestPosition[i]._2.y = pPolygonDestPosition[i]._2.y < fHeight ? fHeight : pPolygonDestPosition[i]._2.y;
			}

			if(fLength3 <= static_cast<float>(m_uiInnerCircleSize))
				pPolygonDestPosition[i]._3.y = static_cast<float>(m_uiHeightValue);
			else if(fLength3 <= static_cast<float>(m_uiOuterCircleSize)
				&& fLength3 > static_cast<float>(m_uiInnerCircleSize)){
				float fHeight = powf((1.f - ((fLength3 - m_uiInnerCircleSize) / static_cast<float>(m_uiOuterCircleSize - m_uiInnerCircleSize))), 0.5f) * m_uiHeightValue;
				pPolygonDestPosition[i]._3.y = pPolygonDestPosition[i]._3.y < fHeight ? fHeight : pPolygonDestPosition[i]._3.y;
			}
		}
	}

	m_pVIBufferTerrainCom->SetVerticesPosition(pPolygonDestPosition, uiPrimitiveCount);

	SafeDelete_Array(pPolygonDestPosition);
	
	return NOERROR;
}

HRESULT CToolTerrain::EditHeightToRect(const _vec3 & _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode){
	if(FAILED(m_pVIBufferTerrainCom->SetVerticesHeightPosition(_vPos, m_uiInnerCircleSize, m_uiOuterCircleSize, static_cast<_float>(m_uiHeightValue), 50.f, _dDeltaTime, _bIsContinuallyMode)))
		return E_FAIL;

	return NOERROR;
}


// 원본 생성 함수(static)
CToolTerrain * CToolTerrain::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CToolTerrain* pInstance = new CToolTerrain(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CToolTerrain Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CGameObject * CToolTerrain::CreateClone(void * _pArgument){
	CToolTerrain* pInstance = new CToolTerrain(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CToolTerrain Clone Failure");
	}

	return pInstance;
}

// Free
void CToolTerrain::Free(){
	SafeRelease(m_pShaderCom);
	SafeRelease(m_pTextureCom);
	SafeRelease(m_pFilterTextureCom);
	SafeRelease(m_pBrushTextureCom);
	SafeRelease(m_pRendererCom);
	SafeRelease(m_pTransformCom);
	SafeRelease(m_pVIBufferTerrainCom);
	SafeRelease(m_pPickeeCom);

	CGameObject::Free();
}
