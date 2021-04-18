#include "stdafx.h"
#include "..\Header\NaviMeshObject.h"
#include "..\Header\ToolStaticObject.h"
#include "..\Header\MainFrm.h"

USING(Tool)

CNaviMeshObject::CNaviMeshObject(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CGameObject(_pGraphicDevice)
	, m_pRenderer(nullptr)
	, m_pShaderCom(nullptr)
	, m_uiPointIndex(0)
	, m_bIsNaviMeshCreate(false)
	, m_bIsNaviMeshVisible(false)
	, m_pListBox(nullptr){
	::ZeroMemory(m_vPoint, sizeof(_vec3*) * 3);
}

CNaviMeshObject::CNaviMeshObject(const CNaviMeshObject & _NaviMeshObject)
	: CGameObject(_NaviMeshObject)
	, m_pRenderer(_NaviMeshObject.m_pRenderer)
	, m_pShaderCom(_NaviMeshObject.m_pShaderCom)
	, m_uiPointIndex(0)
	, m_bIsNaviMeshCreate(false)
	, m_bIsNaviMeshVisible(false)
	, m_pListBox(_NaviMeshObject.m_pListBox){
	SafeAddRef(m_pRenderer);
	SafeAddRef(m_pShaderCom);
	::ZeroMemory(m_vPoint, sizeof(_vec3*) * 3);
}

HRESULT CNaviMeshObject::InitGameObject_Proto(CListBox* _pListBox){
	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pRenderer), L"RendererCom", L"RendererCom_Proto", 0)))
		return E_FAIL;

	if(FAILED(CGameObject::AddComponent(reinterpret_cast<CComponent**>(&m_pShaderCom), L"ShaderCom", L"NaviMeshShaderCom_Proto", 0)))
		return E_FAIL;

	m_pListBox = _pListBox;

	return NOERROR;
}

HRESULT CNaviMeshObject::InitGameObject_Clone(void * _pArgument){
	
	return NOERROR;
}

HRESULT CNaviMeshObject::LateInitGameObject(){

	return NOERROR;
}

HRESULT CNaviMeshObject::UpdateGameObject(_double _dDeltaTime){
	if(m_uiPointIndex == 3){
		
		_vec3 vLine1 = *m_vPoint[1] - *m_vPoint[0];
		_vec3 vLine2 = *m_vPoint[2] - *m_vPoint[1];

		_vec3 vCross = {};
		D3DXVec3Cross(&vCross, &vLine1, &vLine2);

		if(vCross.y < 0){
			_vec3* temp = m_vPoint[1];
			m_vPoint[1] = m_vPoint[2];
			m_vPoint[2] = temp;
		}

		POLYGON_POSITION* pPolygonPos = new POLYGON_POSITION;

		pPolygonPos->_1 = *m_vPoint[0];
		pPolygonPos->_2 = *m_vPoint[1];
		pPolygonPos->_3 = *m_vPoint[2];
		m_listPolygon.push_back(pPolygonPos);

		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
		CGameObject::GetGraphicDevice()->CreateVertexBuffer(sizeof(_VERTEX_COLOR) * 3, 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVertexBuffer, nullptr);

		_VERTEX_COLOR* pVertices = nullptr;
		pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

		pVertices[0].vPos = pPolygonPos->_1;
		pVertices[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertices[1].vPos = pPolygonPos->_2;
		pVertices[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertices[2].vPos = pPolygonPos->_3;
		pVertices[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertexBuffer->Unlock();

		m_listVertexBuffer.push_back(pVertexBuffer);

		for(_uint i = 0; i < 3; ++i)
			SafeDelete(m_vPoint[i]);

		if(nullptr != m_pListBox){
			CString temp;
			temp.Format(L"%d", m_listPolygon.size()-1);
			m_pListBox->AddString(temp);
		}

		m_uiPointIndex = 0;
	}

	return NOERROR;
}

HRESULT CNaviMeshObject::LateUpdateGameObject(_double _dDeltaTime){
	if(nullptr == m_pRenderer)
		return E_FAIL;

	if(m_bIsNaviMeshVisible)
		m_pRenderer->AddRenderList(CRenderer::RENDER_PRE_UI, this);

	return NOERROR;
}

HRESULT CNaviMeshObject::RenderGameObject(_double _dDeltaTime){
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphicDevice = CGameObject::GetGraphicDevice();
	SafeAddRef(pGraphicDevice);

	for(auto& elem : m_listCollider)
		elem->RenderCollider();

	_matrix		matIdentity, matView, matProj;
	pManagement->GetTransform(D3DTS_VIEW, &matView);
	pManagement->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pShaderCom->SetValue("g_matWorld", D3DXMatrixIdentity(&matIdentity), sizeof(_matrix));
	m_pShaderCom->SetValue("g_matView", &matView, sizeof(_matrix));
	m_pShaderCom->SetValue("g_matProj", &matProj, sizeof(_matrix));

	m_pShaderCom->BeginShader();
	m_pShaderCom->BeginPass(0);

	_int iIndex = 0;

	for(auto& elem : m_listVertexBuffer){
		if(iIndex++ == m_iSelectedIndex)
			m_pShaderCom->SetBool("g_bIsChecked", true);
		else
			m_pShaderCom->SetBool("g_bIsChecked", false);

		m_pShaderCom->CommitChange();

		pGraphicDevice->SetStreamSource(0, elem, 0, sizeof(_VERTEX_COLOR));
		pGraphicDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

		pGraphicDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
	}

	m_pShaderCom->EndPass();
	m_pShaderCom->EndShader();

	SafeRelease(pGraphicDevice);
	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CNaviMeshObject::SetPickingPos(_vec3 & _vPos){
	if(!m_bIsNaviMeshCreate)
		return NOERROR;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);

	_bool bHit = false;
	_vec3 vNaviPos = {};

	for(auto& elem : m_listCollider){
		_vec3 vColliderPos = elem->GetPosition();
		if(0.5f >= D3DXVec3Length(&(vColliderPos - _vPos))){
			vNaviPos = vColliderPos;
			bHit = true;
			break;
		}
	}

	if(!bHit)
		vNaviPos = _vPos;

		m_vPoint[m_uiPointIndex] = new _vec3(vNaviPos);

	if(!bHit){
		CCollider::COLLIDER_DESC tDesc;
		_matrix matWorld = {};
		D3DXMatrixTranslation(&matWorld, m_vPoint[m_uiPointIndex]->x, m_vPoint[m_uiPointIndex]->y, m_vPoint[m_uiPointIndex]->z);
		tDesc.pParentWorldMatrix = &matWorld;
		tDesc.vLocalPosition = { 0.f,0.f,0.f };
		tDesc.vScale = { 0.5f, 0.5f, 0.5f };


		CCollider* pCollider = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"Collider_Shpere_Proto", 0, &tDesc));
		if(nullptr != pCollider)
			m_listCollider.push_back(pCollider);
	}
	m_uiPointIndex++;

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CNaviMeshObject::SaveNaviMesh(const _tchar * _szFilePath){
	if(nullptr == _szFilePath)
		return E_FAIL;
		
	HANDLE hFile = CreateFile(_szFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(0 == hFile)
		return E_FAIL;

	_ulong dwByte;

	_uint uiCellCount = m_listPolygon.size();

	WriteFile(hFile, &uiCellCount, sizeof(_ulong), &dwByte, nullptr);
	if(0 == dwByte){
		CloseHandle(hFile);
		return E_FAIL;
	}
	
	for(auto& elem : m_listPolygon){
		POLYGON_POSITION temp = *elem;

		WriteFile(hFile, &temp, sizeof(POLYGON_POSITION), &dwByte, nullptr);
		if(0 == dwByte)
			break;
	}

	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CNaviMeshObject::LoadNaviMesh(const _tchar * _szFilePath){
	if(nullptr == _szFilePath)
		return E_FAIL;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return E_FAIL;
	SafeAddRef(pManagement);


	HANDLE hFile = CreateFile(_szFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(0 == hFile)
		return E_FAIL;
	
	if(nullptr != m_pListBox)
		m_pListBox->ResetContent();

	for(auto& elem : m_listPolygon)
		SafeDelete(elem);
	m_listPolygon.clear();

	for(auto& elem : m_listCollider)
		SafeRelease(elem);
	m_listCollider.clear();

	for(auto& elem : m_listVertexBuffer)
		SafeRelease(elem);
	m_listVertexBuffer.clear();



	_ulong dwByte;

	_uint uiCellCount = m_listPolygon.size();

	ReadFile(hFile, &uiCellCount, sizeof(_ulong), &dwByte, nullptr);
	if(0 == dwByte){
		CloseHandle(hFile);
		return E_FAIL;
	}

	while(true){
		POLYGON_POSITION temp;

		ReadFile(hFile, &temp, sizeof(POLYGON_POSITION), &dwByte, nullptr);
		if(0 == dwByte)
			break;

		m_listPolygon.push_back(new POLYGON_POSITION(temp));



		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
		CGameObject::GetGraphicDevice()->CreateVertexBuffer(sizeof(_VERTEX_COLOR) * 3, 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVertexBuffer, nullptr);

		_VERTEX_COLOR* pVertices = nullptr;
		pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

		pVertices[0].vPos = temp._1;
		pVertices[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertices[1].vPos = temp._2;
		pVertices[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertices[2].vPos = temp._3;
		pVertices[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

		pVertexBuffer->Unlock();

		m_listVertexBuffer.push_back(pVertexBuffer);



		_bool bCheckColider1 = true;
		_bool bCheckColider2 = true;
		_bool bCheckColider3 = true;
		for(list<CCollider*>::iterator iter = m_listCollider.begin(); iter != m_listCollider.end(); ++iter){
			if(temp._1 == (*iter)->GetPosition())
				bCheckColider1 = false;
			if(temp._2 == (*iter)->GetPosition())
				bCheckColider2 = false;
			if(temp._3 == (*iter)->GetPosition())
				bCheckColider3 = false;
		}

		CCollider::COLLIDER_DESC tDesc;
		_matrix matWorld = {};
		tDesc.vLocalPosition = { 0.f,0.f,0.f };
		tDesc.vScale = { 0.5f, 0.5f, 0.5f };
		tDesc.pParentWorldMatrix = &matWorld;

		if(bCheckColider1){
			D3DXMatrixTranslation(&matWorld, temp._1.x, temp._1.y, temp._1.z);
			CCollider* pCollider = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"Collider_Shpere_Proto", 0, &tDesc));
			if(nullptr != pCollider)
				m_listCollider.push_back(pCollider);
		}

		if(bCheckColider2){
			D3DXMatrixTranslation(&matWorld, temp._2.x, temp._2.y, temp._2.z);
			CCollider* pCollider = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"Collider_Shpere_Proto", 0, &tDesc));
			if(nullptr != pCollider)
				m_listCollider.push_back(pCollider);
		}

		if(bCheckColider3){
			D3DXMatrixTranslation(&matWorld, temp._3.x, temp._3.y, temp._3.z);
			CCollider* pCollider = dynamic_cast<CCollider*>(pManagement->AddCloneComponent(L"Collider_Shpere_Proto", 0, &tDesc));
			if(nullptr != pCollider)
				m_listCollider.push_back(pCollider);
		}

		if(nullptr != m_pListBox){
			CString temp;
			temp.Format(L"%d", m_listPolygon.size() - 1);
			m_pListBox->AddString(temp);
		}
	}
	CloseHandle(hFile);

	SafeRelease(pManagement);

	return NOERROR;
}

HRESULT CNaviMeshObject::ReleaseNaviMesh(_int _iIndex){
	list<POLYGON_POSITION*>::iterator iter_Polygon = m_listPolygon.begin();
	list<LPDIRECT3DVERTEXBUFFER9>::iterator iter_VertexBuffer = m_listVertexBuffer.begin();

	for(int i = 0; i < _iIndex; ++i){
		++iter_Polygon;
		++iter_VertexBuffer;
	}
	POLYGON_POSITION temp = **iter_Polygon;

	SafeDelete(*iter_Polygon);
	m_listPolygon.erase(iter_Polygon);

	SafeRelease(*iter_VertexBuffer);
	m_listVertexBuffer.erase(iter_VertexBuffer);

	_bool bCheckColider1 = true;
	_bool bCheckColider2 = true;
	_bool bCheckColider3 = true;

	for(list<POLYGON_POSITION*>::iterator iter = m_listPolygon.begin(); iter != m_listPolygon.end(); ++iter){
		if(temp._1 == (*iter)->_1
			|| temp._1 == (*iter)->_2
			|| temp._1 == (*iter)->_3)
			bCheckColider1 = false;

		if(temp._2 == (*iter)->_1
			|| temp._2 == (*iter)->_2
			|| temp._2 == (*iter)->_3)
			bCheckColider2 = false;

		if(temp._3 == (*iter)->_1
			|| temp._3 == (*iter)->_2
			|| temp._3 == (*iter)->_3)
			bCheckColider3 = false;
	}

	if(bCheckColider1){
		for(list<CCollider*>::iterator iter = m_listCollider.begin(); iter != m_listCollider.end(); ++iter){
			if(temp._1 == (*iter)->GetPosition()){
				SafeRelease(*iter);
				m_listCollider.erase(iter);
				break;
			}
		}
	}

	if(bCheckColider2){
		for(list<CCollider*>::iterator iter = m_listCollider.begin(); iter != m_listCollider.end(); ++iter){
			if(temp._2 == (*iter)->GetPosition()){
				SafeRelease(*iter);
				m_listCollider.erase(iter);
				break;
			}
		}
	}

	if(bCheckColider3){
		for(list<CCollider*>::iterator iter = m_listCollider.begin(); iter != m_listCollider.end(); ++iter){
			if(temp._3 == (*iter)->GetPosition()){
				SafeRelease(*iter);
				m_listCollider.erase(iter);
				break;
			}
		}
	}

	return NOERROR;
}

HRESULT CNaviMeshObject::TrasnformMousePoint(const POINT & _tInMousePos, _vec3& _vOutRayDir, _vec3& _vOutRayPos){
	CManagement* pManagment = CManagement::GetInstance();
	if(nullptr == pManagment)
		return E_FAIL;

	SafeAddRef(pManagment);
	/////////////////////////////////////////////////////////////////////////
	/*뷰포트에서 투영 스페이스로의 변환*/
	// 뷰포트 변수
	D3DVIEWPORT9 ViewPort;
	// 현재 사용되는 뷰포트의 정보를 가져옴
	pManagment->GetGraphicDevice()->GetViewport(&ViewPort);

	// 마우스의 위치벡터는 투영스페이스에서 항상 원점
	_vOutRayPos = { 0.f,0.f,0.f };

	// 투영스페이스에서 마우스의 방향 벡터(-1,1에서 1,-1의 범위에 들어가는 좌표)
	_vOutRayDir.x = _tInMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
	_vOutRayDir.y = _tInMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
	_vOutRayDir.z = 0.f;

	/////////////////////////////////////////////////////////////////////////
	/*투영 스페이스에서 뷰 스페이스로의 변환*/
	_matrix matProj = {};

	// 투영 행렬 얻어옴
	if(FAILED(pManagment->GetTransform(D3DTS_PROJECTION, &matProj))){
		SafeRelease(pManagment);
		return E_FAIL;
	}
	// 투영 행렬의 역행렬 계산
	D3DXMatrixInverse(&matProj, nullptr, &matProj);
	// 투영행렬의 역행렬을 마우스의 방향벡터에 곱함
	D3DXVec3TransformCoord(&_vOutRayDir, &_vOutRayDir, &matProj);

	// D3DXVec3TransformCoord() 함수는 _vec3와 행렬과 곱하는 함수
	// 이때 w의 자리에 1로 채워서 계산을 진행한다.
	// 또 행렬과 곱셈 후 w의 값으로 x, y, z를 나누는 작업도 진행한다.
	// 투영 행렬의 역행력에는 1/(뷰스페이스 상의 z)이 들어있어 곱셈 후 w의 자리엔 1/(뷰스페이스 상의 z)가 들어가게 된다.
	// 따라서 D3DXVec3TransformCoord()를 이용하여 투영 행렬의 역행렬을 곱하면 뷰스페이스 상의 좌표로 복구가 된다.

	/////////////////////////////////////////////////////////////////////////
	/* 뷰 스페이스에서 월드 스페이스로의 변환*/
	_matrix matCameraWorld = {};
	// 카메라의 월드 행렬을 얻어옴(사실 뷰 변환 행렬의 역행렬로 구할 수 있음)
	if(FAILED(pManagment->GetCameraWorldMatrix(&matCameraWorld))){
		SafeRelease(pManagment);
		return E_FAIL;
	}

	// 마우스의 위치 벡터와 카메라의 월드행렬을 곱함(D3DXVec3TransformCoord())
	D3DXVec3TransformCoord(&_vOutRayPos, &_vOutRayPos, &matCameraWorld);
	// 마우스의 방향 벡터와 카메라의 월드행렬을 곱함(D3DXVec3TransformNormal())
	D3DXVec3TransformNormal(&_vOutRayDir, &_vOutRayDir, &matCameraWorld);

	/////////////////////////////////////////////////////////////////////////

	SafeRelease(pManagment);

	return NOERROR;
}

CNaviMeshObject * CNaviMeshObject::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, CListBox* _pListBox){
	CNaviMeshObject* pInstance = new CNaviMeshObject(_pGraphicDevice);
	if(FAILED(pInstance->InitGameObject_Proto(_pListBox))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNaviMeshObject Create Failure");
	}
	
	return pInstance;
}

CGameObject * CNaviMeshObject::CreateClone(void * _pArgument){
	CNaviMeshObject* pInstance = new CNaviMeshObject(*this);
	if(FAILED(pInstance->InitGameObject_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CNaviMeshObject Create Failure");
	}

	return pInstance;
}

void CNaviMeshObject::Free(){
	for(auto& elem : m_listPolygon)
		SafeDelete(elem);
	m_listPolygon.clear();

	for(auto& elem : m_listCollider)
		SafeRelease(elem);
	m_listCollider.clear();

	for(auto& elem : m_listVertexBuffer)
		SafeRelease(elem);
	m_listVertexBuffer.clear();

	for(_uint i = 0; i < 3; ++i)
		SafeDelete(m_vPoint[i]);

	SafeRelease(m_pRenderer);
	SafeRelease(m_pShaderCom);


	CGameObject::Free();
}
