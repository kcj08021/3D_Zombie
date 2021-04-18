// ..\Code\MapToolDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "..\Header\MainFrm.h"
#include "..\Header\MapToolDialog.h"
#include "..\Header\ToolTerrain.h"

// CMapToolDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMapToolDialog, CDialog)

CMapToolDialog::CMapToolDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOLDIALOG, pParent)
	, m_pToolView(nullptr)
	, m_uiSizeX(0), m_uiSizeZ(0), m_fInterval(0.f)
	, m_iShaderPass(0)
	, m_uiHeightValue(0), m_uiOuterRange(0), m_uiInnerRange(0){
}

CMapToolDialog::~CMapToolDialog()
{
}

void CMapToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_SIZE_X_EDIT, m_uiSizeX);
	DDX_Text(pDX, IDC_SIZE_Z_EDIT, m_uiSizeZ);
	DDX_Text(pDX, IDC_INTERVAL_EDIT, m_fInterval);
	DDX_Radio(pDX, IDC_RADIO_WIREFRAME, m_iShaderPass);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_uiHeightValue);
	DDX_Text(pDX, IDC_EDIT_OUTER_RANGE, m_uiOuterRange);
	DDX_Text(pDX, IDC_EDIT_INNER_RANGE, m_uiInnerRange);
}


BEGIN_MESSAGE_MAP(CMapToolDialog, CDialog)
	ON_BN_CLICKED(IDC_SIZE_APPLY, &CMapToolDialog::OnBnClickedSizeApply)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_WIREFRAME, IDC_RADIO_SOLID, &CMapToolDialog::OnBnClickedRenderOption)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_HEIGHT, IDC_EDIT_INNER_RANGE, &CMapToolDialog::OnEnChangedHetghtRangeValue)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_HEIGHT_ONCE, IDC_RAIDO_HEIGHT_CONTINUALLY, &CMapToolDialog::OnBnClickedRadioClickOption)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CURSOR_CIRCLE, IDC_RAIDO_CURSOR_RECT, &CMapToolDialog::OnBnClickedRadioCursorOption)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_SAVE, &CMapToolDialog::OnBnClickedButtonHeightSave)
	ON_BN_CLICKED(IDC_BUTTON_HEIGHT_LOAD, &CMapToolDialog::OnBnClickedButtonHeightLoad)
END_MESSAGE_MAP()


// CMapToolDialog �޽��� ó�����Դϴ�.
BOOL CMapToolDialog::OnInitDialog(){
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	if(nullptr == pMainFrame){
		MSG_BOX(L"CMapToolDialog Initialize Failure");
		return FALSE;
	}

	m_pToolView = pMainFrame->GetToolView();
	if(nullptr == m_pToolView){
		MSG_BOX(L"CMapToolDialog Initialize Failure");
		return FALSE;
	}

	// ���� �ɼ� ���� ��ư �ʱ�ȭ(�⺻�� ���̾�������)
	CButton* pButton = nullptr;
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_WIREFRAME));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);
	m_iShaderPass = 0;

	// Ŭ�� �ɼ� �ʱ�ȭ
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_HEIGHT_ONCE));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);

	// ��� �ɼ� �ʱ�ȭ
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_CURSOR_CIRCLE));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);

	CEdit* pEdit = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_HEIGHT_MAX));
	if(nullptr != pEdit)
		pEdit->SetWindowTextW(L"50");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CMapToolDialog::OnBnClickedRenderOption(UINT msg){
	UpdateData(TRUE);

	m_pToolView->SetTerrainShaderPass(m_iShaderPass);

	UpdateData(TRUE);
}

void CMapToolDialog::OnBnClickedSizeApply(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	if(1 >= m_uiSizeX
		|| 1 >= m_uiSizeZ
		|| 0.5f > m_fInterval){
		UpdateData(FALSE);
		MSG_BOX(L"X, Z���� 2 �̻�, ������ 0.5 �̻��� �Է����ּ���.");
		return;
	}

	CManagement* _pManagement = CManagement::GetInstance();
	if(nullptr == _pManagement)
		return;
	SafeAddRef(_pManagement);

	_pManagement->ReleaseProtoComponent(L"VIBufferTerrain_Proto", 0);

	_pManagement->AddProtoComponent(L"VIBufferTerrain_Proto", 0, CVIBufferTerrain::CreateProto(_pManagement->GetGraphicDevice(), m_uiSizeX, m_uiSizeZ, m_fInterval));

	CToolTerrain* pToolTerrain = m_pToolView->GetToolTerrain();
	if(nullptr != pToolTerrain){
		pToolTerrain->SettingVIBufferTerrain();
	}

	m_pToolView->SetTerrainShaderPass(m_iShaderPass);

	SafeRelease(_pManagement);

	UpdateData(FALSE);
}

void CMapToolDialog::OnEnChangedHetghtRangeValue(UINT msg){
	UpdateData(TRUE);

	if(nullptr == m_pToolView)
		return;

	if(50 < m_uiHeightValue)
		m_uiHeightValue = 50;

	m_pToolView->SetTerrainHeightValue(m_uiHeightValue);
	if(m_uiInnerRange > m_uiOuterRange - 1)
		m_uiInnerRange = m_uiOuterRange - 1;

	m_pToolView->SetTerrainBrushInfo(m_uiOuterRange, m_uiInnerRange);

	UpdateData(FALSE);
}

void CMapToolDialog::OnBnClickedRadioClickOption(UINT msg){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	if(nullptr == m_pToolView)
		return;
	
	CButton* pCheck = nullptr;
	pCheck = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_HEIGHT_ONCE));
	if(nullptr == pCheck)
		return;

	if(pCheck->GetCheck())
		m_pToolView->SetClickOption(false);
	else
		m_pToolView->SetClickOption(true);

	UpdateData(FALSE);
}

void CMapToolDialog::OnBnClickedRadioCursorOption(UINT msg){
	UpdateData(TRUE);

	CButton* pCheck = nullptr;
	pCheck = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_CURSOR_CIRCLE));
	if(nullptr == pCheck)
		return;

	if(pCheck->GetCheck())
		m_pToolView->SetCursorOption(false);
	else
		m_pToolView->SetCursorOption(true);
	
	UpdateData(FALSE);
}

void CMapToolDialog::OnBnClickedButtonHeightSave(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L".bmp", L"Height.bmp",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"32��Ʈ ��Ʈ��|*.bmp;*.dib||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// ���� �۾� ��θ� ������ �Լ�.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// ���� ��ο��� ���ϸ� �����ϴ� �Լ�. ������ ���ϸ��� ������ ���� �������� �����Ѵ�.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Resource\\Texture\\Terrain");

	// ���� ��ȭ���ڿ��� ������ �ʱ� ��� ����.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // ����� X

	if(IDOK == Dlg.DoModal()){
		if(FAILED(m_pToolView->SaveHeightMap(Dlg.GetPathName())))
			MSG_BOX(L"���� ����!");
	}

	UpdateData(FALSE);
}

void CMapToolDialog::OnBnClickedButtonHeightLoad(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L".bmp", L"Height.bmp",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"32��Ʈ ��Ʈ��|*.bmp;*.dib||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// ���� �۾� ��θ� ������ �Լ�.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// ���� ��ο��� ���ϸ� �����ϴ� �Լ�. ������ ���ϸ��� ������ ���� �������� �����Ѵ�.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Resource\\Texture\\Terrain");

	// ���� ��ȭ���ڿ��� ������ �ʱ� ��� ����.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // ����� X

	if(IDOK == Dlg.DoModal()){
		CManagement* _pManagement = CManagement::GetInstance();
		if(nullptr == _pManagement)
			return;
		SafeAddRef(_pManagement);

		_pManagement->ReleaseProtoComponent(L"VIBufferTerrain_Proto", 0);

		_pManagement->AddProtoComponent(L"VIBufferTerrain_Proto", 0, CVIBufferTerrain::CreateProto(_pManagement->GetGraphicDevice(), Dlg.GetPathName(), 1.f));

		CToolTerrain* pToolTerrain = m_pToolView->GetToolTerrain();
		if(nullptr != pToolTerrain){
			pToolTerrain->SettingVIBufferTerrain();
		}


#ifdef _DEBUG
		m_pToolView->SetTerrainShaderPass(m_iShaderPass);
#endif 
		SafeRelease(_pManagement);
	}

	UpdateData(FALSE);
}