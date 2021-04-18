// ..\Code\LightToolDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "..\Header\LightToolDialog.h"
#include "afxdialogex.h"
#include "..\Header\MainFrm.h"


// CLightToolDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLightToolDialog, CDialog)

CLightToolDialog::CLightToolDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LIGHTTOOLDIALOG, pParent), m_fPointRange(0){
	::ZeroMemory(m_fDirectionalDiffuse, sizeof(m_fDirectionalDiffuse));
	::ZeroMemory(m_fDirectionalSpecular, sizeof(m_fDirectionalSpecular));
	::ZeroMemory(m_fDirectionalAmbient, sizeof(m_fDirectionalAmbient));
	::ZeroMemory(m_fDirection, sizeof(m_fDirection));

	::ZeroMemory(m_fPointDiffuse, sizeof(m_fPointDiffuse));
	::ZeroMemory(m_fPointSpecular, sizeof(m_fPointSpecular));
	::ZeroMemory(m_fPointAmbient, sizeof(m_fPointAmbient));
	::ZeroMemory(m_fPointPosition, sizeof(m_fPointPosition));
}

CLightToolDialog::~CLightToolDialog()
{
}

void CLightToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_DIFFUSE_R, m_fDirectionalDiffuse[R]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_DIFFUSE_G, m_fDirectionalDiffuse[G]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_DIFFUSE_B, m_fDirectionalDiffuse[B]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_DIFFUSE_A, m_fDirectionalDiffuse[A]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_SPECULAR_R, m_fDirectionalSpecular[R]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_SPECULAR_G, m_fDirectionalSpecular[G]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_SPECULAR_B, m_fDirectionalSpecular[B]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_SPECULAR_A, m_fDirectionalSpecular[A]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_AMBIENT_R, m_fDirectionalAmbient[R]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_AMBIENT_G, m_fDirectionalAmbient[G]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_AMBIENT_B, m_fDirectionalAmbient[B]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_AMBIENT_A, m_fDirectionalAmbient[A]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_X, m_fDirection[X]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_Y, m_fDirection[Y]);
	DDX_Text(pDX, IDC_EDIT_DIRECTION_Z, m_fDirection[Z]);

	DDX_Text(pDX, IDC_EDIT_POINT_DIFFUSE_R, m_fPointDiffuse[R]);
	DDX_Text(pDX, IDC_EDIT_POINT_DIFFUSE_G, m_fPointDiffuse[G]);
	DDX_Text(pDX, IDC_EDIT_POINT_DIFFUSE_B, m_fPointDiffuse[B]);
	DDX_Text(pDX, IDC_EDIT_POINT_DIFFUSE_A, m_fPointDiffuse[A]);
	DDX_Text(pDX, IDC_EDIT_POINT_SPECULAR_R, m_fPointSpecular[R]);
	DDX_Text(pDX, IDC_EDIT_POINT_SPECULAR_G, m_fPointSpecular[G]);
	DDX_Text(pDX, IDC_EDIT_POINT_SPECULAR_B, m_fPointSpecular[B]);
	DDX_Text(pDX, IDC_EDIT_POINT_SPECULAR_A, m_fPointSpecular[A]);
	DDX_Text(pDX, IDC_EDIT_POINT_AMBIENT_R, m_fPointAmbient[R]);
	DDX_Text(pDX, IDC_EDIT_POINT_AMBIENT_G, m_fPointAmbient[G]);
	DDX_Text(pDX, IDC_EDIT_POINT_AMBIENT_B, m_fPointAmbient[B]);
	DDX_Text(pDX, IDC_EDIT_POINT_AMBIENT_A, m_fPointAmbient[A]);
	DDX_Text(pDX, IDC_EDIT_POINT_POSITION_X, m_fPointPosition[X]);
	DDX_Text(pDX, IDC_EDIT_POINT_POSITION_Y, m_fPointPosition[Y]);
	DDX_Text(pDX, IDC_EDIT_POINT_POSITION_Z, m_fPointPosition[Z]);
	DDX_Text(pDX, IDC_EDIT_POINT_RANGE, m_fPointRange);

	DDX_Control(pDX, IDC_LIST1, m_ListOfPointLight);
}

BOOL CLightToolDialog::OnInitDialog(){
	CDialog::OnInitDialog();

	UpdateData(TRUE);

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	if(nullptr == pMainFrame){
		MSG_BOX(L"CLightToolDialog Initialize Failure");
		return FALSE;
	}

	m_pToolView = pMainFrame->GetToolView();
	if(nullptr == m_pToolView){
		MSG_BOX(L"CLightToolDialog Initialize Failure");
		return FALSE;
	}

	m_pManagement = CManagement::GetInstance();
	if(nullptr == m_pManagement){
			MSG_BOX(L"CLightToolDialog Initialize Failure");
			return FALSE;
	}

	D3DLIGHT9 LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Direction = _vec3(1.f, -1.f, 1.f);

	if(FAILED(m_pManagement->AddLight(m_pToolView->GetDevice(), &LightDesc, &m_pDirectionalLight)))
		return FALSE;

	::memcpy_s(m_fDirectionalDiffuse, sizeof(m_fDirectionalDiffuse), &LightDesc.Diffuse, sizeof(LightDesc.Diffuse));
	::memcpy_s(m_fDirectionalAmbient, sizeof(m_fDirectionalAmbient), &LightDesc.Ambient, sizeof(LightDesc.Ambient));
	::memcpy_s(m_fDirectionalSpecular, sizeof(m_fDirectionalSpecular), &LightDesc.Specular, sizeof(LightDesc.Specular));
	::memcpy_s(m_fDirection, sizeof(m_fDirection), &LightDesc.Direction, sizeof(LightDesc.Direction));

	UpdateData(FALSE);

	return 0;
}

void CLightToolDialog::SetPickingPos(_vec3 _vPos){
	UpdateData(TRUE);

	CButton* pButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_POINT_POSITION_PICKING));
	if(nullptr == pButton)
		return;

	::memcpy_s(m_fPointPosition, sizeof(m_fPointPosition), &_vPos, sizeof(float) * 3);

	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CLightToolDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTION_APPLY, &CLightToolDialog::OnBnClickedButtonDirectionApply)
	ON_BN_CLICKED(IDC_BUTTON_POINT_APPLY, &CLightToolDialog::OnBnClickedButtonPointApply)
	ON_BN_CLICKED(IDC_BUTTON_POINT_DELETE, &CLightToolDialog::OnBnClickedButtonPointDelete)
	ON_BN_CLICKED(IDC_BUTTON_POINT_ADD, &CLightToolDialog::OnBnClickedButtonPointAdd)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_LOAD, &CLightToolDialog::OnBnClickedButtonLightLoad)
	ON_BN_CLICKED(IDC_BUTTON_LIGHT_SAVE, &CLightToolDialog::OnBnClickedButtonLightSave)
	ON_LBN_SELCHANGE(IDC_LIST1, &CLightToolDialog::OnLbnSelchangeList1)
END_MESSAGE_MAP()



void CLightToolDialog::OnBnClickedButtonDirectionApply(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	D3DLIGHT9* pLight = m_pDirectionalLight->GetLight();
	if(nullptr == pLight){
		UpdateData(FALSE);
		return;
	}

	::memcpy_s(&(pLight->Diffuse), sizeof(float) * 4, m_fDirectionalDiffuse, sizeof(m_fDirectionalDiffuse));
	::memcpy_s(&(pLight->Ambient), sizeof(float) * 4, m_fDirectionalAmbient, sizeof(m_fDirectionalAmbient));
	::memcpy_s(&(pLight->Specular), sizeof(float) * 4, m_fDirectionalSpecular, sizeof(m_fDirectionalSpecular));
	::memcpy_s(&(pLight->Direction), sizeof(float) * 3, m_fDirection, sizeof(m_fDirection));

	UpdateData(FALSE);
}


void CLightToolDialog::OnLbnSelchangeList1(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	int iSelectedNum = m_ListOfPointLight.GetCurSel();
	if(0 <= iSelectedNum){
		CLight* pLight = reinterpret_cast<CLight*>(m_ListOfPointLight.GetItemData(iSelectedNum));
		if(nullptr == pLight){
			UpdateData(FALSE);
			return;
		}

		D3DLIGHT9* pLightDesc = pLight->GetLight();
		if(nullptr != pLightDesc){
			::memcpy_s(m_fPointDiffuse, sizeof(m_fPointDiffuse), &pLightDesc->Diffuse, sizeof(float) * 4);
			::memcpy_s(m_fPointAmbient, sizeof(m_fPointAmbient), &pLightDesc->Ambient, sizeof(float) * 4);
			::memcpy_s(m_fPointSpecular, sizeof(m_fPointSpecular), &pLightDesc->Specular, sizeof(float) * 4);
			::memcpy_s(m_fPointPosition, sizeof(m_fPointPosition), &pLightDesc->Position, sizeof(float) * 3);
			m_fPointRange= pLightDesc->Range;
		}
	}

	UpdateData(FALSE);
}


void CLightToolDialog::OnBnClickedButtonPointApply(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	D3DLIGHT9 LightDesc = {};
	int iSelectedNum = m_ListOfPointLight.GetCurSel();

	LightDesc.Type = D3DLIGHT_POINT;
	::memcpy_s(&(LightDesc.Diffuse), sizeof(float) * 4, m_fPointDiffuse, sizeof(m_fPointDiffuse));
	::memcpy_s(&(LightDesc.Ambient), sizeof(float) * 4, m_fPointAmbient, sizeof(m_fPointAmbient));
	::memcpy_s(&(LightDesc.Specular), sizeof(float) * 4, m_fPointSpecular, sizeof(m_fPointSpecular));
	::memcpy_s(&(LightDesc.Position), sizeof(float) * 3, m_fPointPosition, sizeof(m_fPointPosition));
	LightDesc.Range = m_fPointRange;


	if(iSelectedNum >= 0){
		D3DLIGHT9* pLightDesc = reinterpret_cast<CLight*>(m_ListOfPointLight.GetItemData(iSelectedNum))->GetLight();
		::memcpy_s(pLightDesc, sizeof(D3DLIGHT9), &LightDesc, sizeof(D3DLIGHT9));
	}

	UpdateData(FALSE);
}

void CLightToolDialog::OnBnClickedButtonPointAdd(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	D3DLIGHT9 LightDesc = {};

	LightDesc.Type = D3DLIGHT_POINT;
	::memcpy_s(&(LightDesc.Diffuse), sizeof(float) * 4, m_fPointDiffuse, sizeof(m_fPointDiffuse));
	::memcpy_s(&(LightDesc.Ambient), sizeof(float) * 4, m_fPointAmbient, sizeof(m_fPointAmbient));
	::memcpy_s(&(LightDesc.Specular), sizeof(float) * 4, m_fPointSpecular, sizeof(m_fPointSpecular));
	::memcpy_s(&(LightDesc.Position), sizeof(float) * 3, m_fPointPosition, sizeof(m_fPointPosition));
	LightDesc.Range = m_fPointRange;

	CLight* tempLight = nullptr;
	if(FAILED(m_pManagement->AddLight(m_pToolView->GetDevice(), &LightDesc, &tempLight))){
		MSG_BOX(L"���� �߰� ����");
		return;
	}

	CString temp;
	temp.Format(L"%d", m_ListOfPointLight.GetCount());
	int iIndex = m_ListOfPointLight.AddString(temp);
	m_ListOfPointLight.SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(tempLight));

	UpdateData(FALSE);
}


void CLightToolDialog::OnBnClickedButtonPointDelete(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int iSelectedNum = m_ListOfPointLight.GetCurSel();
	if(iSelectedNum >= 0){
		CLight* pLight = reinterpret_cast<CLight*>(m_ListOfPointLight.GetItemData(iSelectedNum));
		if(FAILED(m_pManagement->ReleaseLight(pLight))){
			MSG_BOX(L"���� ���� ����");
			return;
		}
		m_ListOfPointLight.DeleteString(iSelectedNum);

		int iIndex = m_ListOfPointLight.GetCount();
		list<CLight*> tempList;

		for(int i = 0; i < iIndex; ++i)
			tempList.push_back(reinterpret_cast<CLight*>(m_ListOfPointLight.GetItemData(i)));

		m_ListOfPointLight.ResetContent();

		CString temp;
		for(auto& elem : tempList){
			temp.Format(L"%d", m_ListOfPointLight.GetCount());
			iIndex = m_ListOfPointLight.AddString(temp);
			m_ListOfPointLight.SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(elem));
		}
	} 
}

void CLightToolDialog::OnBnClickedButtonLightSave(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L".LIGHTDAT", L"Light.LIGHTDAT",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Light Data File|*.LIGHTDAT||",
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
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

		DWORD dwByte = 0;
		WriteFile(hFile, m_pDirectionalLight->GetLight(), sizeof(D3DLIGHT9), &dwByte, nullptr);
		if(0 == dwByte){
			CloseHandle(hFile);
			UpdateData(FALSE);
			return;
		}

		for(int i = 0; i < m_ListOfPointLight.GetCount(); ++i){
			D3DLIGHT9* pLight = reinterpret_cast<CLight*>(m_ListOfPointLight.GetItemData(i))->GetLight();

			WriteFile(hFile, pLight, sizeof(D3DLIGHT9), &dwByte, nullptr);
			if(0 == dwByte){
				break;
			}
		}
		
		CloseHandle(hFile);
	}
	UpdateData(FALSE);
}


void CLightToolDialog::OnBnClickedButtonLightLoad(){
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L".LIGHTDAT", L"Light.LIGHTDAT",
		OFN_HIDEREADONLY,
		L"Light Data File|*.LIGHTDAT||",
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
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(0 == hFile){
			UpdateData(FALSE);
			return;
		}

		D3DLIGHT9 tlight = {};

		DWORD dwByte = 0;
		ReadFile(hFile, &tlight, sizeof(D3DLIGHT9), &dwByte, nullptr);
		if(0 == dwByte){
			CloseHandle(hFile);
			UpdateData(FALSE);
			return;
		}

		m_pManagement->ResetLight();

		if(FAILED(m_pManagement->AddLight(m_pToolView->GetDevice(), &tlight, &m_pDirectionalLight))){
			CloseHandle(hFile);
			UpdateData(FALSE);
			return;
		}

		::memcpy_s(m_fDirectionalDiffuse, sizeof(m_fDirectionalDiffuse), &tlight.Diffuse, sizeof(tlight.Diffuse));
		::memcpy_s(m_fDirectionalAmbient, sizeof(m_fDirectionalAmbient), &tlight.Ambient, sizeof(tlight.Ambient));
		::memcpy_s(m_fDirectionalSpecular, sizeof(m_fDirectionalSpecular), &tlight.Specular, sizeof(tlight.Specular));
		::memcpy_s(m_fDirection, sizeof(m_fDirection), &tlight.Direction, sizeof(tlight.Direction));



		m_ListOfPointLight.ResetContent();
		
		while(true){
			DWORD dwByte = 0;
			ReadFile(hFile, &tlight, sizeof(D3DLIGHT9), &dwByte, nullptr);
			if(0 == dwByte){
				break;
			}

			CLight* tempLight = nullptr;
			if(FAILED(m_pManagement->AddLight(m_pToolView->GetDevice(), &tlight, &tempLight))){
				MSG_BOX(L"���� �߰� ����");
				break;
			}

			int iIndex = 0;

			CString temp;
			temp.Format(L"%d", m_ListOfPointLight.GetCount());

			iIndex = m_ListOfPointLight.AddString(temp);
			m_ListOfPointLight.SetItemData(iIndex, reinterpret_cast<DWORD_PTR>(tempLight));
		}

		CloseHandle(hFile);
	}

	UpdateData(FALSE);
}

