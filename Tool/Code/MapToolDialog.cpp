// ..\Code\MapToolDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "..\Header\MainFrm.h"
#include "..\Header\MapToolDialog.h"
#include "..\Header\ToolTerrain.h"

// CMapToolDialog 대화 상자입니다.

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


// CMapToolDialog 메시지 처리기입니다.
BOOL CMapToolDialog::OnInitDialog(){
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

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

	// 렌더 옵션 라디오 버튼 초기화(기본값 와이어프레임)
	CButton* pButton = nullptr;
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_WIREFRAME));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);
	m_iShaderPass = 0;

	// 클릭 옵션 초기화
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_HEIGHT_ONCE));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);

	// 모양 옵션 초기화
	pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_CURSOR_CIRCLE));
	if(nullptr != pButton)
		pButton->SetCheck(TRUE);

	CEdit* pEdit = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_HEIGHT_MAX));
	if(nullptr != pEdit)
		pEdit->SetWindowTextW(L"50");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapToolDialog::OnBnClickedRenderOption(UINT msg){
	UpdateData(TRUE);

	m_pToolView->SetTerrainShaderPass(m_iShaderPass);

	UpdateData(TRUE);
}

void CMapToolDialog::OnBnClickedSizeApply(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if(1 >= m_uiSizeX
		|| 1 >= m_uiSizeZ
		|| 0.5f > m_fInterval){
		UpdateData(FALSE);
		MSG_BOX(L"X, Z값은 2 이상, 간격은 0.5 이상을 입력해주세요.");
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L".bmp", L"Height.bmp",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"32비트 비트맵|*.bmp;*.dib||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 작업 경로를 얻어오는 함수.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// 현재 경로에서 파일명 제거하는 함수. 제거할 파일명이 없으면 말단 폴더명을 제거한다.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Resource\\Texture\\Terrain");

	// 현재 대화상자에서 보여질 초기 경로 설정.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // 상대경로 X

	if(IDOK == Dlg.DoModal()){
		if(FAILED(m_pToolView->SaveHeightMap(Dlg.GetPathName())))
			MSG_BOX(L"저장 실패!");
	}

	UpdateData(FALSE);
}

void CMapToolDialog::OnBnClickedButtonHeightLoad(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L".bmp", L"Height.bmp",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"32비트 비트맵|*.bmp;*.dib||",
		this);

	TCHAR szCurrentPath[MAX_PATH] = L"";

	// 현재 작업 경로를 얻어오는 함수.
	::GetCurrentDirectory(MAX_PATH, szCurrentPath);

	// 현재 경로에서 파일명 제거하는 함수. 제거할 파일명이 없으면 말단 폴더명을 제거한다.
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Resource\\Texture\\Terrain");

	// 현재 대화상자에서 보여질 초기 경로 설정.
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath; // 상대경로 X

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