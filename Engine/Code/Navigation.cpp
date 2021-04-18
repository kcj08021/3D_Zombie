#include "..\Header\Navigation.h"
#include "..\Header\Cell.h"
#include "..\Header\Management.h"


// 생성자
CNavigation::CNavigation(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_uiCellCount(0)
	, m_uiCurrentIndex(0)\
	, m_pShaderCom(nullptr){
}

// 복사 생성자
CNavigation::CNavigation(const CNavigation & _Navigation)
	: CComponent(_Navigation)
	, m_vecCell(_Navigation.m_vecCell)
	, m_uiCellCount(_Navigation.m_uiCellCount)
	, m_pShaderCom(_Navigation.m_pShaderCom)
	, m_uiCurrentIndex(_Navigation.m_uiCurrentIndex){
	SafeAddRef(m_pShaderCom);
	// 벡터의 얕은 복사(담긴 CCell들의 레퍼런스 카운트만 증가)
	for(auto& elem : m_vecCell)
		SafeAddRef(elem);
}

// Initialize with Prototype
HRESULT CNavigation::InitComponent_Proto(const _tchar * _szFilePath){
	if(nullptr == _szFilePath)
		return E_FAIL;

	m_pShaderCom = CShader::CreateProto(CComponent::GetGraphicDevice(), L"../Bin/ShaderFile/NaviMeshShader.fx");
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	// 파일 불러오기
	HANDLE hFile = ::CreateFile(_szFilePath, GENERIC_READ, 0 ,nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(0 == hFile)
		return E_FAIL;

	_ulong dwReadByte;

	// 가장 처음에 총 삼각형 갯수를 로드함
	::ReadFile(hFile, &m_uiCellCount, sizeof(_uint), &dwReadByte, nullptr);
	if(dwReadByte == 0)
		return E_FAIL;

	// 갯수 만큼 벡터 크기 예약
	m_vecCell.reserve(m_uiCellCount);

	// 삼각형 정보 읽어옴
	for(_uint i = 0; i < m_uiCellCount; ++i){
		POLYGON_POSITION tPolygon;

		::ReadFile(hFile, &tPolygon, sizeof(POLYGON_POSITION), &dwReadByte, nullptr);
		if(0 == dwReadByte)
			break;

		CCell* pCell = CCell::CreateCell(CComponent::GetGraphicDevice(), tPolygon , i);
		if(nullptr == pCell)
			break;

		m_vecCell.push_back(pCell);
	}

	// 읽어온 삼각형들의 이웃을 세팅
	SetttingNeigbor();

	return NOERROR;
}	

// Initialize with Clone
HRESULT CNavigation::InitComponent_Clone(void * _pArgument){
	if(nullptr != _pArgument){
		_vec3 vPosition = *reinterpret_cast<_vec3*>(_pArgument);
		const CCell* pNeighbor = nullptr;

		for(_uint index = 0; index < m_vecCell.size(); ++index){
			if(m_vecCell[index]->CheckInside(vPosition, &pNeighbor)){
				m_uiCurrentIndex = index;
				break;
			}
		}
	}

	return NOERROR;
}

_bool CNavigation::MovingCheck(_vec3 & _vMoveResult){
	const CCell* pNeighbor = nullptr;
	_uint uiIndex = m_uiCurrentIndex;
	if(!m_vecCell[uiIndex]->CheckInside(_vMoveResult, &pNeighbor)){
		do{
			if(nullptr == pNeighbor){
				m_uiCurrentIndex = uiIndex;
				return false;
			}
			
			uiIndex = pNeighbor->GetIndex();
		}while(!m_vecCell[uiIndex]->CheckInside(_vMoveResult, &pNeighbor));
	}

	m_uiCurrentIndex = uiIndex;

	return true;
}

HRESULT CNavigation::RenderNaviMash(){
	if(nullptr == m_pShaderCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);
	
	_matrix matWorld, matView, matProj;

	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pShaderCom->SetValue("g_matWorld", D3DXMatrixIdentity(&matWorld), sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));
	m_pShaderCom->SetBool("g_bIsChecked", false);

	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	for(auto& elem : m_vecCell)
		elem->RenderCell();

	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pManagement);
	return NOERROR;
}

// 각 삼각형들의 이웃을 세팅하는 함수
HRESULT CNavigation::SetttingNeigbor(){
	for(_uint i = 0; i < m_uiCellCount; ++i){
		for(_uint j = 0; j < m_uiCellCount; ++j){
			if(m_vecCell[i] == m_vecCell[j])
				continue;
			// i의 _1과 _2점을 공유하는 삼각형을 찾음
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._1, m_vecCell[i]->GetPolygonPos()._2))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_1_2, m_vecCell[j]);

			// i의 _2과 _3점을 공유하는 삼각형을 찾음
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._2, m_vecCell[i]->GetPolygonPos()._3))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_2_3, m_vecCell[j]);

			// i의 _3과 _1점을 공유하는 삼각형을 찾음
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._3, m_vecCell[i]->GetPolygonPos()._1))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_3_1, m_vecCell[j]);
		}
	}

	return NOERROR;
}

// 원본 생성 함수(static)
CNavigation * CNavigation::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar * _szFilePath){
	CNavigation* pInstance = new CNavigation(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szFilePath))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNavigation Create Failure");
	}
	
	return pInstance;
}

// 복제 생성 함수
CComponent * CNavigation::CreateClone(void * _pArgument){
	CNavigation* pInstance = new CNavigation(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNavigation Create Failure");
	}

	return pInstance;
}

// Free
void CNavigation::Free(){
	for(auto& elem : m_vecCell)
		SafeRelease(elem);
	m_vecCell.clear();

	SafeRelease(m_pShaderCom);

	CComponent::Free();
}
