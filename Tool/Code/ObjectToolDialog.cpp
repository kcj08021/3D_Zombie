// ..\Code\ObjectToolDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "..\Header\ObjectToolDialog.h"

#include "..\Header\MainFrm.h"
#include "..\Header\ToolView.h"
#include "..\Header\ToolStaticObject.h"
#include "..\Header\ToolCamera.h"

#include "Management.h"

#include "afxdialogex.h"


// CObjectToolDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectToolDialog, CDialog)

CObjectToolDialog::CObjectToolDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTTOOLDIALOG, pParent)
	, m_pToolView(nullptr)
	, m_strLayer(_T("")), m_strCloneObject(_T("")){
	::ZeroMemory(m_fScale, sizeof(m_fScale));
	::ZeroMemory(m_fRotate, sizeof(m_fRotate));
	::ZeroMemory(m_fPostion, sizeof(m_fPostion));
}

CObjectToolDialog::~CObjectToolDialog()
{
	for(auto& elem : m_LoadLayerKey){
		SafeDelete(elem.second);
	}
}

HRESULT CObjectToolDialog::LoadPrototypeObject(){
	return E_NOTIMPL;
}

void CObjectToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCALE_X, m_fScale[X]);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y, m_fScale[Y]);
	DDX_Text(pDX, IDC_EDIT_SCALE_Z, m_fScale[Z]);
	DDX_Text(pDX, IDC_EDIT_ROTATE_X, m_fRotate[X]);
	DDX_Text(pDX, IDC_EDIT_ROTATE_Y, m_fRotate[Y]);
	DDX_Text(pDX, IDC_EDIT_ROTATE_Z, m_fRotate[Z]);
	DDX_Text(pDX, IDC_EDIT_POSITION_X, m_fPostion[X]);
	DDX_Text(pDX, IDC_EDIT_POSITION_Y, m_fPostion[Y]);
	DDX_Text(pDX, IDC_EDIT_POSITION_Z, m_fPostion[Z]);
	DDX_Control(pDX, IDC_LIST_PROTOTYPE, m_PrototypeListBox);
	DDX_Control(pDX, IDC_TREE_CLONE, m_CloneTreeControl);
	DDX_Text(pDX, IDC_EDIT_LAYER, m_strLayer);
	DDX_Text(pDX, IDC_EDIT_OBJECTNAME, m_strCloneObject);
}

void CObjectToolDialog::SetPickingPos(_vec3 & _vPos){
	UpdateData(TRUE);

	if(m_bIsFileDialogOpen)
		return;

	CButton* pCheckButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_POSITION_PICKING));
	if(nullptr == pCheckButton)
		return;

	HTREEITEM SelectedItem = m_CloneTreeControl.GetSelectedItem();
	if(nullptr == SelectedItem)
		return;

	CToolStaticObject* pGameObject = reinterpret_cast<CToolStaticObject*>(m_CloneTreeControl.GetItemData(SelectedItem));

	if(pCheckButton->GetCheck()
		&& nullptr != pGameObject){
		m_fPostion[X] = _vPos.x;
		m_fPostion[Y] = _vPos.y;
		m_fPostion[Z] = _vPos.z;

		CToolStaticObject::STATIC_MESH_DESC tDesc = {};

		tDesc.tSettingInfo.fScaleX = m_fScale[X];
		tDesc.tSettingInfo.fScaleY = m_fScale[Y];
		tDesc.tSettingInfo.fScaleZ = m_fScale[Z];
		tDesc.tSettingInfo.fRotateX = m_fRotate[X];
		tDesc.tSettingInfo.fRotateY = m_fRotate[Y];
		tDesc.tSettingInfo.fRotateZ = m_fRotate[Z];
		tDesc.tSettingInfo.fPosX = m_fPostion[X];
		tDesc.tSettingInfo.fPosY = m_fPostion[Y];
		tDesc.tSettingInfo.fPosZ = m_fPostion[Z];

		pGameObject->SetInfo(tDesc.tSettingInfo);
	}
	
	UpdateData(FALSE);
}


BEGIN_MESSAGE_MAP(CObjectToolDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_SAVE, &CObjectToolDialog::OnBnClickedButtonObjectSave)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_LOAD, &CObjectToolDialog::OnBnClickedButtonObjectLoad)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_SCALE_X, IDC_EDIT_POSITION_Z, &CObjectToolDialog::OnEnChangeEditSetting)
	ON_BN_CLICKED(IDC_CHECK_FIXED_SCALE_RATIO, &CObjectToolDialog::OnBnClickedCheckFixedScaleRatio)
	ON_BN_CLICKED(IDC_BUTTON_CLONE_ADD, &CObjectToolDialog::OnBnClickedButtonCloneAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLONE_REMOVE, &CObjectToolDialog::OnBnClickedButtonCloneRemove)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CLONE, &CObjectToolDialog::OnTvnSelchangedTreeClone)
END_MESSAGE_MAP()


HRESULT CObjectToolDialog::AddStaticMeshPrototype(CManagement* _pManagement){
	SafeAddRef(_pManagement);

	if(!FAILED(_pManagement->AddProtoComponent(L"car1_Proto", 0, CStaticMesh::CreateProto(_pManagement->GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car1/", L"car1.x")))){
		m_PrototypeListBox.AddString(L"car1_Proto");
	}

	if(!FAILED(_pManagement->AddProtoComponent(L"car2_Proto", 0, CStaticMesh::CreateProto(_pManagement->GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car2/", L"car2.x")))){
		m_PrototypeListBox.AddString(L"car2_Proto");
	}

	if(!FAILED(_pManagement->AddProtoComponent(L"car3_Proto", 0, CStaticMesh::CreateProto(_pManagement->GetGraphicDevice(), L"../../Resource/Meshes/StaticMesh/car3/", L"car3.x")))){
		m_PrototypeListBox.AddString(L"car3_Proto");
	}

	SafeRelease(_pManagement);

	return NOERROR;
}

HTREEITEM CObjectToolDialog::FindTreeItem(const _tchar * _szItem, HTREEITEM _hItem){
	if(nullptr == _hItem)
		return nullptr;

	HTREEITEM ReturnItem = nullptr;

	if(!lstrcmp(_szItem, m_CloneTreeControl.GetItemText(_hItem))){
		ReturnItem = _hItem;
	} else{
		ReturnItem = FindTreeItem(_szItem, m_CloneTreeControl.GetNextSiblingItem(_hItem));
	}

	return ReturnItem;
}

// CObjectToolDialog 메시지 처리기입니다.
BOOL CObjectToolDialog::OnInitDialog(){
	CDialog::OnInitDialog();

	UpdateData(TRUE);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	if(nullptr == pMainFrame){
		MSG_BOX(L"CObjectToolDialog Initialize Failure");
		return FALSE;
	}

	m_pToolView = pMainFrame->GetToolView();
	if(nullptr == m_pToolView){
		MSG_BOX(L"CObjectToolDialog Initialize Failure");
		return FALSE;
	}

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement){
		MSG_BOX(L"CObjectToolDialog Initialize Failure");
		return FALSE;
	}
	SafeAddRef(pManagement);

	CButton* pCheckButton = nullptr;
	
	pCheckButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_FIXED_SCALE_RATIO));
	
	if(nullptr != pCheckButton){
		pCheckButton->SetCheck(TRUE);

		CEdit* pEditScaleY = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_SCALE_Y));
		CEdit* pEditScaleZ = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_SCALE_Z));
	
		if(nullptr != pEditScaleZ || nullptr != pEditScaleY){
			pEditScaleY->SetReadOnly(TRUE);
			pEditScaleZ->SetReadOnly(TRUE);
		}
	}

	AddStaticMeshPrototype(pManagement);

	m_fScale[X] = m_fScale[Y] = m_fScale[Z] = 1.f;

	UpdateData(FALSE);


	SafeRelease(pManagement);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CObjectToolDialog::OnBnClickedButtonCloneAdd(){
	UpdateData(TRUE);
	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement){
		MSG_BOX(L"Object Clone Add Failure");
		return;
	}
	SafeAddRef(pManagement);
	_int iSelectedIndex = m_PrototypeListBox.GetCurSel();
	if(iSelectedIndex < 0){
		MSG_BOX(L"원본이 선택되지 않았습니다.");
		SafeRelease(pManagement);
		UpdateData(FALSE);
		return;
	}
		
	CString SelectedItem;
	m_PrototypeListBox.GetText(iSelectedIndex, SelectedItem);



	const _tchar* szLayer = m_strLayer.GetString();
	const _tchar* szObject = m_strCloneObject.GetString();

	if(0 >=::lstrlen(szLayer)){
		MSG_BOX(L"레이어 이름을 입력해주세요.");
		SafeRelease(pManagement);
		UpdateData(FALSE);
		return;
	} else if(0 >= ::lstrlen(szObject)){
		MSG_BOX(L"복제된 오브젝트의 이름을 입력해주세요.");
		SafeRelease(pManagement);
		UpdateData(FALSE);
		return;
	}

	CToolStaticObject::STATIC_MESH_DESC tDesc = {};

	tDesc.tSettingInfo.fScaleX = m_fScale[X];
	tDesc.tSettingInfo.fScaleY = m_fScale[Y];
	tDesc.tSettingInfo.fScaleZ = m_fScale[Z];
	tDesc.tSettingInfo.fRotateX = m_fRotate[X];
	tDesc.tSettingInfo.fRotateY = m_fRotate[Y];
	tDesc.tSettingInfo.fRotateZ = m_fRotate[Z];
	tDesc.tSettingInfo.fPosX = m_fPostion[X];
	tDesc.tSettingInfo.fPosY = m_fPostion[Y];
	tDesc.tSettingInfo.fPosZ = m_fPostion[Z];
	lstrcpy(tDesc.tSettingInfo.szProto, SelectedItem.GetString());


	CGameObject* pGameObj = nullptr;
	pManagement->AddCloneObjectToLayer(L"StaticMesh_Proto", 0, szLayer, &tDesc, &pGameObj);

	if(nullptr == pGameObj){
		MSG_BOX(L"오브젝트 복제 실패");
		SafeRelease(pManagement);
		UpdateData(FALSE);
		return;
	}

	HTREEITEM LayerItem = FindTreeItem(szLayer, m_CloneTreeControl.GetFirstVisibleItem());

	if(nullptr == LayerItem){
		LayerItem = m_CloneTreeControl.InsertItem(szLayer);
	}		
		
	HTREEITEM ObjectItem = m_CloneTreeControl.InsertItem(szObject, LayerItem);
	m_CloneTreeControl.SetItemData(ObjectItem, reinterpret_cast<DWORD_PTR>(pGameObj));

	SafeRelease(pManagement);

	UpdateData(FALSE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CObjectToolDialog::OnBnClickedButtonCloneRemove(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement){
		MSG_BOX(L"복제 제거 실패");
		return;
	}
	SafeAddRef(pManagement);

	HTREEITEM ObjectItem = m_CloneTreeControl.GetSelectedItem();
	if(nullptr == ObjectItem)
		return;

	CGameObject* pGameObj = reinterpret_cast<CGameObject*>(m_CloneTreeControl.GetItemData(ObjectItem));

	if(nullptr == pGameObj){
		if(FAILED(pManagement->ReleaseLayer(0, m_CloneTreeControl.GetItemText(ObjectItem)))){
			MSG_BOX(L"복제 제거 실패");
		} else{
			LAYER_TCHAR_MAP::iterator find_iter = find_if(m_LoadLayerKey.begin(), m_LoadLayerKey.end(), IsEqual_tchar(m_CloneTreeControl.GetItemText(ObjectItem)));
			if(m_LoadLayerKey.end() != find_iter){
				SafeDelete(find_iter->second);
				m_LoadLayerKey.erase(find_iter);
			}
			m_CloneTreeControl.DeleteItem(ObjectItem);
		}
	} else{
		if(FAILED(pManagement->ReleaseCloneObject(0, m_CloneTreeControl.GetItemText(ObjectItem), pGameObj))){
			MSG_BOX(L"복제 제거 실패");
		} else{
			m_CloneTreeControl.DeleteItem(ObjectItem);
		}
	}

	SafeRelease(pManagement);
}

void CObjectToolDialog::OnBnClickedCheckFixedScaleRatio(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pCheckButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_POSITION_PICKING));
	if(nullptr == pCheckButton)
		return;

	CEdit* pEditScaleY = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_SCALE_Y));
	CEdit* pEditScaleZ = reinterpret_cast<CEdit*>(GetDlgItem(IDC_EDIT_SCALE_Z));

	if(nullptr == pEditScaleZ || nullptr == pEditScaleY)
		return;

	if(pCheckButton->GetCheck()){
		pEditScaleY->SetReadOnly(TRUE);
		pEditScaleZ->SetReadOnly(TRUE);

		m_fScale[Z] = m_fScale[Y] = m_fScale[X];
	}
	else{
		pEditScaleY->SetReadOnly(FALSE);
		pEditScaleZ->SetReadOnly(FALSE);
	}
}

void CObjectToolDialog::OnEnChangeEditSetting(UINT _msg){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	bool bIsFixedRatio = false;

	CButton* pCheckButton = nullptr;

	pCheckButton = reinterpret_cast<CButton*>(GetDlgItem(IDC_CHECK_FIXED_SCALE_RATIO));
	if(nullptr != pCheckButton){
		bIsFixedRatio = pCheckButton->GetCheck() == TRUE ? true : false;
	}

	if(bIsFixedRatio){
		m_fScale[Z] = m_fScale[Y] = m_fScale[X];
	}


	CToolStaticObject* pGameObject = reinterpret_cast<CToolStaticObject*>(m_CloneTreeControl.GetItemData(m_CloneTreeControl.GetSelectedItem()));
	if(nullptr == pGameObject){
		UpdateData(FALSE);
		return;
	}

	CToolStaticObject::STATIC_MESH_DESC tDesc = {};

	tDesc.tSettingInfo.fScaleX = m_fScale[X];
	tDesc.tSettingInfo.fScaleY = m_fScale[Y];
	tDesc.tSettingInfo.fScaleZ = m_fScale[Z];
	tDesc.tSettingInfo.fRotateX = m_fRotate[X];
	tDesc.tSettingInfo.fRotateY = m_fRotate[Y];
	tDesc.tSettingInfo.fRotateZ = m_fRotate[Z];
	tDesc.tSettingInfo.fPosX = m_fPostion[X];
	tDesc.tSettingInfo.fPosY = m_fPostion[Y];
	tDesc.tSettingInfo.fPosZ = m_fPostion[Z];

	pGameObject->SetInfo(tDesc.tSettingInfo);

	UpdateData(FALSE);
}


void CObjectToolDialog::OnBnClickedButtonObjectSave(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_bIsFileDialogOpen = true;

	CFileDialog Dlg(FALSE, L".STTOBJ", L"StaticObjectData.STTOBJ",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"오브젝트 배치 데이터|*.STTOBJ||",
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
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if(0 == hFile){
			MSG_BOX(L"저장 실패");
			m_bIsFileDialogOpen = false;
			return;
		}
		_ulong dwByte;

		HTREEITEM ParentItem = m_CloneTreeControl.GetFirstVisibleItem();
		if(nullptr == ParentItem)
			return;

		while(true){
			if(nullptr == ParentItem)
				break;

			HTREEITEM ChildItem = m_CloneTreeControl.GetChildItem(ParentItem);
			while(true){
				if(nullptr == ChildItem)
					break;
				
				CToolStaticObject* pGameObject = reinterpret_cast<CToolStaticObject*>(m_CloneTreeControl.GetItemData(ChildItem));
				if(nullptr == pGameObject){
					break;
				}

				const OBJECT_SETTING_INFO& tInfo = pGameObject->GetInfo();
				_tchar temp[128];
				CString str;
				str += m_CloneTreeControl.GetItemText(ParentItem);
				str += L"|";
				str += m_CloneTreeControl.GetItemText(ChildItem);
				str += L"|";

				swprintf_s(temp, L"%f|", tInfo.fScaleX);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fScaleY);
				str += temp; 
				swprintf_s(temp, L"%f|", tInfo.fScaleZ);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fRotateX);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fRotateY);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fRotateZ);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fPosX);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fPosY);
				str += temp;
				swprintf_s(temp, L"%f|", tInfo.fPosZ);
				str += temp;
				
				str += tInfo.szProto;
				str += L"\n";

				WriteFile(hFile, str, str.GetLength() * sizeof(_tchar), &dwByte, nullptr);
				if(0 == dwByte)
					break;

				ChildItem = m_CloneTreeControl.GetNextSiblingItem(ChildItem);
			}
			ParentItem = m_CloneTreeControl.GetNextSiblingItem(ParentItem);
		}
		CloseHandle(hFile);
	}
	m_bIsFileDialogOpen = false;
}


void CObjectToolDialog::OnBnClickedButtonObjectLoad(){
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_bIsFileDialogOpen = true;

	CManagement* pManagement = CManagement::GetInstance();
	if(nullptr == pManagement){
		MSG_BOX(L"Object Clone Add Failure");
		m_bIsFileDialogOpen = false;
		return;
	}
	SafeAddRef(pManagement);

	CFileDialog Dlg(TRUE, L".STTOBJ", L"StaticObjectData.STTOBJ",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"오브젝트 배치 데이터|*.STTOBJ||",
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
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if(0 == hFile){
			MSG_BOX(L"열기 실패");
			m_bIsFileDialogOpen = false;
			return;
		}
		HTREEITEM item = m_CloneTreeControl.GetFirstVisibleItem();

		while(item != nullptr){
			pManagement->ReleaseLayer(0, m_CloneTreeControl.GetItemText(item));
			HTREEITEM old_item = item;
			item = m_CloneTreeControl.GetNextSiblingItem(item);
			m_CloneTreeControl.DeleteItem(old_item);
		}

		for(auto& elem : m_LoadLayerKey)
			SafeDelete(elem.second);
		m_LoadLayerKey.clear();

		_ulong dwByte;
		_bool bReadFinish = false;
		_tchar szInfo[12][128];
		_uint uiIndex = 0;
		while(true){
			CString str;
			while(true){
				_tchar temp;
				ReadFile(hFile, &temp, sizeof(_tchar), &dwByte, 0);
				if(dwByte == 0){
					bReadFinish = true;
					break;
				}
				
				if(temp == L'|' || temp == L'\n'){
					lstrcpy(szInfo[uiIndex++], str);
					str = L"";
				} else{
					str += temp;
				}

				if(temp == L'\n')
					break;
			}
			uiIndex = 0;
			if(!bReadFinish){


				CToolStaticObject::STATIC_MESH_DESC tDesc = {};

				tDesc.tSettingInfo.fScaleX = static_cast<float>(_ttof(szInfo[2]));
				tDesc.tSettingInfo.fScaleY = static_cast<float>(_ttof(szInfo[3]));
				tDesc.tSettingInfo.fScaleZ = static_cast<float>(_ttof(szInfo[4]));
				tDesc.tSettingInfo.fRotateX = static_cast<float>(_ttof(szInfo[5]));
				tDesc.tSettingInfo.fRotateY = static_cast<float>(_ttof(szInfo[6]));
				tDesc.tSettingInfo.fRotateZ = static_cast<float>(_ttof(szInfo[7]));
				tDesc.tSettingInfo.fPosX = static_cast<float>(_ttof(szInfo[8]));
				tDesc.tSettingInfo.fPosY = static_cast<float>(_ttof(szInfo[9]));
				tDesc.tSettingInfo.fPosZ = static_cast<float>(_ttof(szInfo[10]));
				lstrcpy(tDesc.tSettingInfo.szProto, szInfo[11]);

				_tchar* szLayer = nullptr;
				LAYER_TCHAR_MAP::iterator find_iter = find_if(m_LoadLayerKey.begin(), m_LoadLayerKey.end(), IsEqual_tchar(szInfo[0]));
				if(m_LoadLayerKey.end() == find_iter){
					szLayer = new _tchar[128];
					lstrcpy(szLayer, szInfo[0]);
					m_LoadLayerKey[szLayer] = szLayer;
				} else{
					szLayer = find_iter->second;
				}
				
				CGameObject* pGameObj = nullptr;
				pManagement->AddCloneObjectToLayer(L"StaticMesh_Proto", 0, szLayer, &tDesc, &pGameObj);

				if(nullptr == pGameObj){
					MSG_BOX(L"오브젝트 복제 실패");
					SafeRelease(pManagement);
					UpdateData(FALSE);
					return;
				}

				

				HTREEITEM LayerItem = FindTreeItem(szInfo[0], m_CloneTreeControl.GetFirstVisibleItem());

				if(nullptr == LayerItem){
					LayerItem = m_CloneTreeControl.InsertItem(szInfo[0]);
				}

				HTREEITEM ObjectItem = m_CloneTreeControl.InsertItem(szInfo[1], LayerItem);
				m_CloneTreeControl.SetItemData(ObjectItem, reinterpret_cast<DWORD_PTR>(pGameObj));
			} else
				break;
		}
		CloseHandle(hFile);
	}

	SafeRelease(pManagement);

	UpdateData(FALSE);

	m_bIsFileDialogOpen = false;
}

void CObjectToolDialog::OnTvnSelchangedTreeClone(NMHDR *pNMHDR, LRESULT *pResult){
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateData(TRUE);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	CToolStaticObject* pGameObject = reinterpret_cast<CToolStaticObject*>(m_CloneTreeControl.GetItemData(m_CloneTreeControl.GetSelectedItem()));
	if(nullptr == pGameObject){
		return;
	}
	const OBJECT_SETTING_INFO& tInfo = pGameObject->GetInfo();

	_vec3 vPos = _vec3(tInfo.fPosX, tInfo.fPosY, tInfo.fPosZ);

	m_fScale[X] = tInfo.fScaleX;
	m_fScale[Y] = tInfo.fScaleY;
	m_fScale[Z] = tInfo.fScaleZ;
	m_fRotate[X] = tInfo.fRotateX;
	m_fRotate[Y] = tInfo.fRotateY;
	m_fRotate[Z] = tInfo.fRotateZ;
	m_fPostion[X] = tInfo.fPosX;
	m_fPostion[Y] = tInfo.fPosY;
	m_fPostion[Z] = tInfo.fPosZ;

	CToolCamera* pCamera = m_pToolView->GetCamera();
	if(nullptr != pCamera)
		pCamera->SetFocusPos(vPos);

	UpdateData(FALSE);
}
