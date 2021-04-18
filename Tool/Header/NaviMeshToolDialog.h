#pragma once

// CNaviMeshToolDialog ��ȭ �����Դϴ�.
BEGIN(Tool)
class CToolStaticObject;
class CNaviMeshObject;
END

class CToolView;
class CNaviMeshToolDialog : public CDialog
{
	DECLARE_DYNAMIC(CNaviMeshToolDialog)

public:
	CNaviMeshToolDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNaviMeshToolDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEVIMESHTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CToolView* m_pToolView = nullptr;
	CToolStaticObject* m_pStaticMeshObject = nullptr;
	CNaviMeshObject* m_pNaviMeshObject = nullptr;
	CListBox m_NaviMeshList;
	// ���� ���̾�αװ� �����ִ��� üũ
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
