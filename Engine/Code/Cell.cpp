#include "..\Header\Cell.h"
#include "..\Header\Pipeline.h"

CCell::CCell(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: m_pGraphicDevice(_pGraphicDevice)
	, m_uiIndex(0){
	SafeAddRef(m_pGraphicDevice);
	::ZeroMemory(&m_tPolygon, sizeof(POLYGON_POSITION));
	::ZeroMemory(m_NeighborCell, sizeof(CCell*)*NEIGHBOR_END);
	::ZeroMemory(&m_vDirection, sizeof(_vec3)* LINE_END);
	::ZeroMemory(&m_vNormal, sizeof(_vec3) * LINE_END);
}

HRESULT CCell::InitCell(const POLYGON_POSITION & _refPolygon, _uint _uiIndex){
	m_tPolygon = _refPolygon;

	if(FAILED(m_pGraphicDevice->CreateVertexBuffer(sizeof(_VERTEX_COLOR) * 3, 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &m_pVertexBuffer, nullptr)))
		return E_FAIL;

	_VERTEX_COLOR* pVertices;
	m_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPos = m_tPolygon._1;
	pVertices[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertices[1].vPos = m_tPolygon._2;
	pVertices[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertices[2].vPos = m_tPolygon._3;
	pVertices[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVertexBuffer->Unlock();


	// 면의 각 선을 미리 구해놓음
	m_vDirection[LINE_1_2] = m_tPolygon._2 - m_tPolygon._1;
	m_vDirection[LINE_2_3] = m_tPolygon._3 - m_tPolygon._2;
	m_vDirection[LINE_3_1] = m_tPolygon._1 - m_tPolygon._3;

	// 각 선의 수직인 선을 미리 구해놓음
	m_vNormal[LINE_1_2] = _vec3(m_vDirection[LINE_1_2].z * -1.f, 0.f, m_vDirection[LINE_1_2].x);
	m_vNormal[LINE_2_3] = _vec3(m_vDirection[LINE_2_3].z * -1.f, 0.f, m_vDirection[LINE_2_3].x);
	m_vNormal[LINE_3_1] = _vec3(m_vDirection[LINE_3_1].z * -1.f, 0.f, m_vDirection[LINE_3_1].x);
	
	for(_uint i = 0; i < LINE_END; ++i){
		D3DXVec3Normalize(&m_vNormal[i], &m_vNormal[i]);
	}

	m_uiIndex = _uiIndex;

	return NOERROR;
}

_bool CCell::CheckInside(_vec3 & _vMovingResult, const CCell ** _pNeighbor) const{
	_vec3 vResultPosToPoint[LINE_END];

	D3DXVec3Normalize(&vResultPosToPoint[LINE_1_2], &(_vMovingResult - m_tPolygon._1));
	D3DXVec3Normalize(&vResultPosToPoint[LINE_2_3], &(_vMovingResult - m_tPolygon._2));
	D3DXVec3Normalize(&vResultPosToPoint[LINE_3_1], &(_vMovingResult - m_tPolygon._3));

	for(_uint i = 0; i < LINE_END; ++i){
		if(0 < D3DXVec3Dot(&m_vNormal[i], &vResultPosToPoint[i])){
			*_pNeighbor = m_NeighborCell[i];

		/*	if(nullptr == *_pNeighbor){
				_vMovingResult = vResultPosToPoint[i] - (-m_vNormal[i]) *  D3DXVec3Dot(&vResultPosToPoint[i], &(-m_vNormal[i]));
			}*/

			return false;
		}
	}

	return true;
}

_bool CCell::ComparePoint(const _vec3 _vSrc, const _vec3 _vDest) const{
	if(m_tPolygon._1 == _vSrc){
		if(m_tPolygon._2 == _vDest)
			return true;

		if(m_tPolygon._3 == _vDest)
			return true;
	}

	if(m_tPolygon._2 == _vSrc){
		if(m_tPolygon._3 == _vDest)
			return true;

		if(m_tPolygon._1 == _vDest)
			return true;
	}

	if(m_tPolygon._3 == _vSrc){
		if(m_tPolygon._1 == _vDest)
			return true;

		if(m_tPolygon._2 == _vDest)
			return true;
	}

	return false;
}

HRESULT CCell::RenderCell() const{
	m_pGraphicDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(_VERTEX_COLOR));
	m_pGraphicDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	m_pGraphicDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

	return NOERROR;
}

CCell * CCell::CreateCell(LPDIRECT3DDEVICE9 _pGraphicDevice, const POLYGON_POSITION& _refPolygon, _uint _uiIndex){
	CCell* pInstance = new CCell(_pGraphicDevice);
	if(FAILED(pInstance->InitCell(_refPolygon, _uiIndex))){
		SafeRelease(pInstance);
		MSG_BOX(L"CCell Create Failure");
	}

	return pInstance;
}

void CCell::Free(){
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pGraphicDevice);
}
