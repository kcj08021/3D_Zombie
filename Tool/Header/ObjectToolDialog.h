#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CToolView;

// CObjectToolDialog ��ȭ �����Դϴ�.

class CObjectToolDialog : public CDialog
{
private:
	enum{ X = 0, Y, Z, AXIS_END };
private:
	DECLARE_DYNAMIC(CObjectToolDialog)

public:
	CObjectToolDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CObjectToolDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTOOLDIALOG };
#endif

public:
	// ���ӿ� ���Ǵ� ������Ʈ���� ��� �ε���
	HRESULT LoadPrototypeObject();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	void SetPickingPos(_vec3& _vPos);

private:
	// ToolView
	CToolView* m_pToolView = nullptr;

	// ������Ʈ���� ������ ǥ�õǴ� ���
	CListBox m_PrototypeListBox;
	// ������Ʈ���� �������� ǥ�õǴ� Ʈ��
	CTreeCtrl m_CloneTreeControl;

	// ���̾� �̸�
	CString m_strLayer;
	// ������ ������Ʈ�� �̸�
	CString m_strCloneObject;

	// ȸ�� �� ����	
	_float m_fScale[AXIS_END];
	// ȸ�� �� ����
	_float m_fRotate[AXIS_END];
	// ��ġ �� ����
	_float m_fPostion[AXIS_END];

	// �ε��� ���̾� Ű�� �����ϴ� ����
	map<_tchar*, _tchar*> m_LoadLayerKey;
	typedef map<_tchar*, _tchar*> LAYER_TCHAR_MAP;
	// ���� ���̾�αװ� �����ִ��� üũ
	_bool m_bIsFileDialogOpen = false;

private:
	HRESULT AddStaticMeshPrototype(CManagement* _pManagement);
	HTREEITEM FindTreeItem(const _tchar* _szItem, HTREEITEM _hItem);
private:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonCloneAdd();
	afx_msg void OnBnClickedButtonCloneRemove();

	afx_msg void OnBnClickedCheckFixedScaleRatio();
	afx_msg void OnEnChangeEditSetting(UINT _msg);

	afx_msg void OnBnClickedButtonObjectSave();
	afx_msg void OnBnClickedButtonObjectLoad();
	afx_msg void OnTvnSelchangedTreeClone(NMHDR *pNMHDR, LRESULT *pResult);
};
