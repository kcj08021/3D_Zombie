#include "..\Header\Navigation.h"
#include "..\Header\Cell.h"
#include "..\Header\Management.h"


// ������
CNavigation::CNavigation(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_uiCellCount(0)
	, m_uiCurrentIndex(0)\
	, m_pShaderCom(nullptr){
}

// ���� ������
CNavigation::CNavigation(const CNavigation & _Navigation)
	: CComponent(_Navigation)
	, m_vecCell(_Navigation.m_vecCell)
	, m_uiCellCount(_Navigation.m_uiCellCount)
	, m_pShaderCom(_Navigation.m_pShaderCom)
	, m_uiCurrentIndex(_Navigation.m_uiCurrentIndex){
	SafeAddRef(m_pShaderCom);
	// ������ ���� ����(��� CCell���� ���۷��� ī��Ʈ�� ����)
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

	// ���� �ҷ�����
	HANDLE hFile = ::CreateFile(_szFilePath, GENERIC_READ, 0 ,nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(0 == hFile)
		return E_FAIL;

	_ulong dwReadByte;

	// ���� ó���� �� �ﰢ�� ������ �ε���
	::ReadFile(hFile, &m_uiCellCount, sizeof(_uint), &dwReadByte, nullptr);
	if(dwReadByte == 0)
		return E_FAIL;

	// ���� ��ŭ ���� ũ�� ����
	m_vecCell.reserve(m_uiCellCount);

	// �ﰢ�� ���� �о��
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

	// �о�� �ﰢ������ �̿��� ����
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

// �� �ﰢ������ �̿��� �����ϴ� �Լ�
HRESULT CNavigation::SetttingNeigbor(){
	for(_uint i = 0; i < m_uiCellCount; ++i){
		for(_uint j = 0; j < m_uiCellCount; ++j){
			if(m_vecCell[i] == m_vecCell[j])
				continue;
			// i�� _1�� _2���� �����ϴ� �ﰢ���� ã��
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._1, m_vecCell[i]->GetPolygonPos()._2))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_1_2, m_vecCell[j]);

			// i�� _2�� _3���� �����ϴ� �ﰢ���� ã��
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._2, m_vecCell[i]->GetPolygonPos()._3))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_2_3, m_vecCell[j]);

			// i�� _3�� _1���� �����ϴ� �ﰢ���� ã��
			if(m_vecCell[j]->ComparePoint(m_vecCell[i]->GetPolygonPos()._3, m_vecCell[i]->GetPolygonPos()._1))
				m_vecCell[i]->SettingNeighbor(CCell::NEIGHBOR_3_1, m_vecCell[j]);
		}
	}

	return NOERROR;
}

// ���� ���� �Լ�(static)
CNavigation * CNavigation::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar * _szFilePath){
	CNavigation* pInstance = new CNavigation(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szFilePath))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNavigation Create Failure");
	}
	
	return pInstance;
}

// ���� ���� �Լ�
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
