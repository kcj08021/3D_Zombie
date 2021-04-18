// ToolFormView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"

#include "Management.h"

#include "..\Header\ToolFormView.h"
#include "..\Header\MainFrm.h"
#include "..\Header\ToolView.h"

#include "..\Header\ToolStaticObject.h"

#include "..\Header\MapToolDialog.h"
#include "..\Header\ObjectToolDialog.h"
#include "..\Header\NaviMeshToolDialog.h"
#include "..\Header\LightToolDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CToolFormView

IMPLEMENT_DYNCREATE(CToolFormView, CFormView)

CToolFormView::CToolFormView()
	: CFormView(IDD_TOOLFORMVIEW)
	, m_pMapToolDialog(nullptr)
	, m_pObjectToolDialog(nullptr)
	, m_pPickeeCom(nullptr)
	, m_pMapObject(nullptr)
{

}

CToolFormView::~CToolFormView()
{
	SafeDelete(m_pMapToolDialog);
	SafeDelete(m_pObjectToolDialog);
	SafeDelete(m_pNaviMeshToolDialog);
	SafeDelete(m_pLightToolDialog);
	SafeRelease(m_pMapObject);
	SafeRelease(m_pPickeeCom);
}

void CToolFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
}

void CToolFormView::OnInitialUpdate(){
	CFormView::OnInitialUpdate();



	RECT rc;
	GetClientRect(&rc);

	//m_TabControl.InsertItem(0, L"지형");
	//m_pMapToolDialog = new CMapToolDialog;
	//m_pMapToolDialog->Create(IDD_MAPTOOLDIALOG, &m_TabControl);
	//m_pMapToolDialog->MoveWindow(1, 20, rc.right - 10, rc.bottom - 30);
	//m_pMapToolDialog->ShowWindow(SW_SHOW);

	m_TabControl.InsertItem(0, L"네비메쉬");
	m_pNaviMeshToolDialog = new CNaviMeshToolDialog;
	m_pNaviMeshToolDialog->Create(IDD_NAVIMESHTOOLDIALOG, &m_TabControl);
	m_pNaviMeshToolDialog->MoveWindow(1, 20, rc.right - 10, rc.bottom - 30);
	m_pNaviMeshToolDialog->ShowWindow(SW_SHOW);

	m_TabControl.InsertItem(1, L"오브젝트");
	m_pObjectToolDialog = new CObjectToolDialog;
	m_pObjectToolDialog->Create(IDD_OBJECTTOOLDIALOG, &m_TabControl);
	m_pObjectToolDialog->MoveWindow(1, 20, rc.right - 10, rc.bottom - 30);
	m_pObjectToolDialog->ShowWindow(SW_HIDE);

	m_TabControl.InsertItem(2, L"라이트");
	m_pLightToolDialog = new CLightToolDialog;
	m_pLightToolDialog->Create(IDD_LIGHTTOOLDIALOG, &m_TabControl);
	m_pLightToolDialog->MoveWindow(1, 20, rc.right - 10, rc.bottom - 30);
	m_pLightToolDialog->ShowWindow(SW_HIDE);

	D3DXMatrixIdentity(&m_IdentityMatrix);
}

BEGIN_MESSAGE_MAP(CToolFormView, CFormView)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CToolFormView::OnTcnSelchangeTab1)
ON_BN_CLICKED(IDC_BUTTON_MAP_LOAD, &CToolFormView::OnBnClickedButtonMapLoad)
END_MESSAGE_MAP()


// CToolFormView 진단입니다.

#ifdef _DEBUG
void CToolFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CToolFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CToolFormView 메시지 처리기입니다.

void CToolFormView::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSelect = m_TabControl.GetCurSel();
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	switch(iSelect){
	//case 0:		// 맵툴
	//	m_pMapToolDialog->ShowWindow(SW_SHOW);
	//	m_pObjectToolDialog->ShowWindow(SW_HIDE);
	//	m_pNaviMeshToolDialog->ShowWindow(SW_HIDE);
	//	break;
	case 0:		// 네비메쉬툴
		//m_pMapToolDialog->ShowWindow(SW_HIDE);
		m_pObjectToolDialog->ShowWindow(SW_HIDE);
		m_pNaviMeshToolDialog->ShowWindow(SW_SHOW);
		m_pLightToolDialog->ShowWindow(SW_HIDE);
		m_uiToolIndex = 0;
		break;
	case 1:		// 오브젝트툴
		//m_pMapToolDialog->ShowWindow(SW_HIDE);
		m_pObjectToolDialog->ShowWindow(SW_SHOW);
		m_pNaviMeshToolDialog->ShowWindow(SW_HIDE);
		m_pLightToolDialog->ShowWindow(SW_HIDE);
		m_uiToolIndex = 1;
		break;
	case 2:
		m_pObjectToolDialog->ShowWindow(SW_HIDE);
		m_pNaviMeshToolDialog->ShowWindow(SW_HIDE);
		m_pLightToolDialog->ShowWindow(SW_SHOW);
		m_uiToolIndex = 2;
	default:
		break;
	}

	*pResult = 0;
}


void CToolFormView::OnBnClickedButtonMapLoad(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L".x", L"map.x",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"DirectX File|*.x||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 작업 경로를 얻어오는 함수.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// 현재 경로에서 파일명 제거하는 함수. 제거할 파일명이 없으면 말단 폴더명을 제거한다.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Resource\\Meshes\\StaticMesh\\\shipment\\");

	// 현재 대화상자에서 보여질 초기 경로 설정.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // 상대경로 X

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return;
	SafeAddRef(pManagement);

	if(IDOK == Dlg.DoModal()){
		_tchar szFilePath[MAX_PATH] = L"";
		_tchar szFileName[MAX_PATH] = L"";

		::lstrcpy(szFilePath, Dlg.GetPathName());
		::lstrcpy(szFileName, Dlg.GetFileName());


		::PathRemoveFileSpec(szFilePath);
		::lstrcat(szFilePath, L"\\");

		_matrix mat = {};
		D3DXMatrixIdentity(&mat);

		//D3DXMatrixRotationX(&mat, D3DXToRadian(90.f));

		if(FAILED(pManagement->AddProtoComponent(szFileName, 0, CStaticMesh::CreateProto(pManagement->GetGraphicDevice(), szFilePath, szFileName, &mat)))){
			SafeRelease(pManagement);
			return;
		}

		CToolStaticObject::STATIC_MESH_DESC tDesc = {};
		lstrcpy(tDesc.tSettingInfo.szProto, szFileName);

		CGameObject* pGameObj = nullptr;

		if(FAILED(pManagement->AddCloneObjectToLayer(L"StaticMesh_Proto", 0, L"StaticMesh_Layer", &tDesc, &pGameObj))){
			SafeRelease(pManagement);
			return;
		}

		m_pMapObject = dynamic_cast<CToolStaticObject*>(pGameObj);


		CPickee::PICKEE_DESC PickeeDesc = {};
		PickeeDesc.pGameObj = m_pMapObject;
		PickeeDesc.pMesh = m_pMapObject->GetMesh();
		PickeeDesc.pWolrdMatrix = &m_IdentityMatrix;

		if(nullptr == m_pPickeeCom){
			m_pPickeeCom = dynamic_cast<CPickee*>(pManagement->AddCloneComponent(L"PickeeCom_Proto", 0, &PickeeDesc));
		} else
			m_pPickeeCom->ChangePickeeDesc(&PickeeDesc);
	}
	SafeRelease(pManagement);

	UpdateData(FALSE);
}

void CToolFormView::SetPickingPos(_vec3& _vPos){
	switch(m_uiToolIndex){
	case 0:	
		m_pNaviMeshToolDialog->SetPickingPos(_vPos);
		break;
	case 1:		// 오브젝트툴
		m_pObjectToolDialog->SetPickingPos(_vPos);
		break;
	case 2:
		m_pLightToolDialog->SetPickingPos(_vPos);
		break;
	default:
		break;
	}
}
