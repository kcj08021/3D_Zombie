// ..\Code\NeviMeshToolDialog.cpp : ���� �����Դϴ�.
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

// CNeviMeshToolDialog ��ȭ �����Դϴ�.

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


// CNeviMeshToolDialog �޽��� ó�����Դϴ�.

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

	// ��� �ɼ� �ʱ�ȭ
	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_NAVIMESH_VISIBLE));
	if(nullptr != pButton){
		pButton->SetCheck(TRUE);
		m_pNaviMeshObject->SetIsNaviMeshVisible(true);
	}
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CNaviMeshToolDialog::OnBnClickedNavimeshSaveButton(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	m_bIsFileDialogOpen = true;

	CFileDialog Dlg(FALSE, L".dat", L"Navigation.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data File|*.dat||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// ���� �۾� ��θ� ������ �Լ�.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// ���� ��ο��� ���ϸ� �����ϴ� �Լ�. ������ ���ϸ��� ������ ���� �������� �����Ѵ�.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\");

	// ���� ��ȭ���ڿ��� ������ �ʱ� ��� ����.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // ����� X

	if(IDOK == Dlg.DoModal()){
		if(FAILED(m_pNaviMeshObject->SaveNaviMesh(Dlg.GetPathName()))){
			MSG_BOX(L"���� ����");
		}
	}

    m_bIsFileDialogOpen = false;
	UpdateData(FALSE);
}


void CNaviMeshToolDialog::OnBnClickedNavimeshLoadButton(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	m_bIsFileDialogOpen = true;


	CFileDialog Dlg(TRUE, L".dat", L"Navigation.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data File|*.dat||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// ���� �۾� ��θ� ������ �Լ�.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// ���� ��ο��� ���ϸ� �����ϴ� �Լ�. ������ ���ϸ��� ������ ���� �������� �����Ѵ�.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\");

	// ���� ��ȭ���ڿ��� ������ �ʱ� ��� ����.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // ����� X

	if(IDOK == Dlg.DoModal()){

		if(FAILED(m_pNaviMeshObject->LoadNaviMesh(Dlg.GetPathName()))){
			MSG_BOX(L"�ҷ����� ����");
		}
	}

	m_bIsFileDialogOpen = false;

	UpdateData(FALSE);
}


void CNaviMeshToolDialog::OnBnClickedCheckNavimeshEdit(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(nullptr == m_pNaviMeshObject)
		return;

	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_NAVIMESH_VISIBLE));
	if(nullptr == pButton)
		return;

	m_pNaviMeshObject->SetIsNaviMeshVisible(pButton->GetCheck() == TRUE ? true : false);
}


void CNaviMeshToolDialog::OnBnClickedNavimeshDeleteButton(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int iIndex = m_NaviMeshList.GetCurSel();
	if(0 > iIndex)
		return;
	

	if(FAILED(m_pNaviMeshObject->ReleaseNaviMesh(iIndex)))
		return;

	m_NaviMeshList.DeleteString(static_cast<_uint>(iIndex));
}


void CNaviMeshToolDialog::OnLbnSelchangeNavimeshList(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	_int iIndex = m_NaviMeshList.GetCurSel();

	m_pNaviMeshObject->SetSelectedIndex(iIndex);
}
