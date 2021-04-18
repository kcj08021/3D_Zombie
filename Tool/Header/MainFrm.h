
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "..\Header\ToolView.h"
#include "..\Header\ToolFormView.h"

namespace Engine{
	class CManagement;
};

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
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

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CStatusBar        m_wndStatusBar;

private:
	CSplitterWnd m_MainSpliterWnd;


// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


