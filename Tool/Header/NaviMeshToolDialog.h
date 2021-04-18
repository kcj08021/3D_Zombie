#pragma once

// CNaviMeshToolDialog 대화 상자입니다.
BEGIN(Tool)
class CToolStaticObject;
class CNaviMeshObject;
END

class CToolView;
class CNaviMeshToolDialog : public CDialog
{
	DECLARE_DYNAMIC(CNaviMeshToolDialog)

public:
	CNaviMeshToolDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CNaviMeshToolDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEVIMESHTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CToolView* m_pToolView = nullptr;
	CToolStaticObject* m_pStaticMeshObject = nullptr;
	CNaviMeshObject* m_pNaviMeshObject = nullptr;
	CListBox m_NaviMeshList;
	// 파일 다이얼로그가 열려있는지 체크
	_bool m_bIsFileDialogOpen = false;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNavimeshLoadButton();
	afx_msg void OnBnClickedNavimeshSaveButton();
	afx_msg void OnBnClickedCheckNavimeshVisible();
	afx_msg void OnBnClickedCheckNavimeshEdit();

public:
	void SetPickingPos(_vec3& _vPos);
	afx_msg void OnBnClickedNavimeshDeleteButton();
	afx_msg void OnLbnSelchangeNavimeshList();
};
