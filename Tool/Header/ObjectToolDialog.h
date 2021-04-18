#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CToolView;

// CObjectToolDialog 대화 상자입니다.

class CObjectToolDialog : public CDialog
{
private:
	enum{ X = 0, Y, Z, AXIS_END };
private:
	DECLARE_DYNAMIC(CObjectToolDialog)

public:
	CObjectToolDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectToolDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTOOLDIALOG };
#endif

public:
	// 게임에 사용되는 오브젝트들을 모두 로드함
	HRESULT LoadPrototypeObject();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetPickingPos(_vec3& _vPos);

private:
	// ToolView
	CToolView* m_pToolView = nullptr;

	// 오브젝트들의 원본이 표시되는 목록
	CListBox m_PrototypeListBox;
	// 오브젝트들의 복제들이 표시되는 트리
	CTreeCtrl m_CloneTreeControl;

	// 레이어 이름
	CString m_strLayer;
	// 복제된 오브젝트의 이름
	CString m_strCloneObject;

	// 회전 값 변수	
	_float m_fScale[AXIS_END];
	// 회전 값 변수
	_float m_fRotate[AXIS_END];
	// 위치 값 변수
	_float m_fPostion[AXIS_END];

	// 로드한 레이어 키를 보관하는 변수
	map<_tchar*, _tchar*> m_LoadLayerKey;
	typedef map<_tchar*, _tchar*> LAYER_TCHAR_MAP;
	// 파일 다이얼로그가 열려있는지 체크
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
