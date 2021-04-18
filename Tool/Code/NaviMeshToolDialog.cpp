// ..\Code\NeviMeshToolDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "..\Header\MainFrm.h"
#include "..\Header\ToolView.h"

#include "..\Header\NaviMeshToolDialog.h"
#include "..\Header\ToolStaticObject.h"
#include "..\Header\NaviMeshObject.h"
#include "afxdialogex.h"

USING(Tool)

// CNeviMeshToolDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNaviMeshToolDialog, CDialog)

CNaviMeshToolDialog::CNaviMeshToolDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAVIMESHTOOLDIALOG, pParent)
{

}

CNaviMeshToolDialog::~CNaviMeshToolDialog()
{
}

void CNaviMeshToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAVIMESH_LIST, m_NaviMeshList);
}


BEGIN_MESSAGE_MAP(CNaviMeshToolDialog, CDialog)
	ON_BN_CLICKED(IDC_NAVIMESH_LOAD_BUTTON, &CNaviMeshToolDialog::OnBnClickedNavimeshLoadButton)
	ON_BN_CLICKED(IDC_NAVIMESH_SAVE_BUTTON, &CNaviMeshToolDialog::OnBnClickedNavimeshSaveButton)
	ON_BN_CLICKED(IDC_CHECK_NAVIMESH_VISIBLE, &CNaviMeshToolDialog::OnBnClickedCheckNavimeshVisible)
	ON_BN_CLICKED(IDC_CHECK_NAVIMESH_EDIT, &CNaviMeshToolDialog::OnBnClickedCheckNavimeshEdit)
	ON_BN_CLICKED(IDC_NAVIMESH_DELETE_BUTTON, &CNaviMeshToolDialog::OnBnClickedNavimeshDeleteButton)
	ON_LBN_SELCHANGE(IDC_NAVIMESH_LIST, &CNaviMeshToolDialog::OnLbnSelchangeNavimeshList)
END_MESSAGE_MAP()


// CNeviMeshToolDialog 메시지 처리기입니다.

BOOL CNaviMeshToolDialog::OnInitDialog(){
	CDialog::OnInitDialog();

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	m_pToolView = pMainFrm->GetToolView();

	if(nullptr == m_pToolView)
		return FALSE;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement)
		return FALSE;
	SafeAddRef(pManagement);

	pManagement->AddProtoComponent(L"NaviMeshShaderCom_Proto", 0, CShader::CreateProto(pManagement->GetGraphicDevice(), L"../Bin/ShaderFile/NaviMeshShader.fx"));

	pManagement->AddProtoObject(L"CNaviMeshObject_Proto", CNaviMeshObject::CreateProto(pManagement->GetGraphicDevice(), &m_NaviMeshList));
	pManagement->AddCloneObjectToLayer(L"CNaviMeshObject_Proto", 0, L"NaviMesh", nullptr, reinterpret_cast<CGameObject**>(&m_pNaviMeshObject));

	SafeRelease(pManagement);

	// 모양 옵션 초기화
	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_NAVIMESH_VISIBLE));
	if(nullptr != pButton){
		pButton->SetCheck(TRUE);
		m_pNaviMeshObject->SetIsNaviMeshVisible(true);
	}
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNaviMeshToolDialog::OnBnClickedNavimeshSaveButton(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_bIsFileDialogOpen = true;

	CFileDialog Dlg(FALSE, L".dat", L"Navigation.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data File|*.dat||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 작업 경로를 얻어오는 함수.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// 현재 경로에서 파일명 제거하는 함수. 제거할 파일명이 없으면 말단 폴더명을 제거한다.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\");

	// 현재 대화상자에서 보여질 초기 경로 설정.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // 상대경로 X

	if(IDOK == Dlg.DoModal()){
		if(FAILED(m_pNaviMeshObject->SaveNaviMesh(Dlg.GetPathName()))){
			MSG_BOX(L"저장 실패");
		}
	}

    m_bIsFileDialogOpen = false;
	UpdateData(FALSE);
}


void CNaviMeshToolDialog::OnBnClickedNavimeshLoadButton(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_bIsFileDialogOpen = true;


	CFileDialog Dlg(TRUE, L".dat", L"Navigation.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data File|*.dat||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 작업 경로를 얻어오는 함수.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// 현재 경로에서 파일명 제거하는 함수. 제거할 파일명이 없으면 말단 폴더명을 제거한다.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\");

	// 현재 대화상자에서 보여질 초기 경로 설정.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // 상대경로 X

	if(IDOK == Dlg.DoModal()){

		if(FAILED(m_pNaviMeshObject->LoadNaviMesh(Dlg.GetPathName()))){
			MSG_BOX(L"불러오기 실패");
		}
	}

	m_bIsFileDialogOpen = false;

	UpdateData(FALSE);
}


void CNaviMeshToolDialog::OnBnClickedCheckNavimeshEdit(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(nullptr == m_pNaviMeshObject)
		return;

	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_NAVIMESH_EDIT));
	if(nullptr == pButton)
		return;

	m_pNaviMeshObject->SetIsNaviMeshCreate(pButton->GetCheck() == TRUE ? true : false);
}

void CNaviMeshToolDialog::SetPickingPos(_vec3 & _vPos){
	if(!m_bIsFileDialogOpen)
		m_pNaviMeshObject->SetPickingPos(_vPos);
}



void CNaviMeshToolDialog::OnBnClickedCheckNavimeshVisible(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(nullptr == m_pNaviMeshObject)
		return;

	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_NAVIMESH_VISIBLE));
	if(nullptr == pButton)
		return;

	m_pNaviMeshObject->SetIsNaviMeshVisible(pButton->GetCheck() == TRUE ? true : false);
}


void CNaviMeshToolDialog::OnBnClickedNavimeshDeleteButton(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iIndex = m_NaviMeshList.GetCurSel();
	if(0 > iIndex)
		return;
	

	if(FAILED(m_pNaviMeshObject->ReleaseNaviMesh(iIndex)))
		return;

	m_NaviMeshList.DeleteString(static_cast<_uint>(iIndex));
}


void CNaviMeshToolDialog::OnLbnSelchangeNavimeshList(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_NaviMeshList.GetCurSel();

	m_pNaviMeshObject->SetSelectedIndex(iIndex);
}
