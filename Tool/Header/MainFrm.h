
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "..\Header\ToolView.h"
#include "..\Header\ToolFormView.h"

namespace Engine{
	class CManagement;
};

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CToolView* GetToolView() const{
		return dynamic_cast<CToolView*>(m_MainSpliterWnd.GetPane(0,1));
	}
	CToolFormView* GetFormView() const{
		return dynamic_cast<CToolFormView*>(m_MainSpliterWnd.GetPane(0,0));
	}

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar        m_wndStatusBar;

private:
	CSplitterWnd m_MainSpliterWnd;


// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


