#pragma once
#include "afxcmn.h"

namespace Tool{
	class CToolStaticObject;
};
using namespace Tool;

class CMapToolDialog;
class CObjectToolDialog;
class CNaviMeshToolDialog;
class CLightToolDialog;

// CToolFormView 폼 뷰입니다.

class CToolFormView : public CFormView
{
	DECLARE_DYNCREATE(CToolFormView)

public:
	enum DIALOG_TYPE{ NAVI, OBJECT};

protected:
	CToolFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CToolFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOLFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	// 툴 종류를 고르기 위한 탭 컨트롤 변수
	CTabCtrl m_TabControl;
	CToolStaticObject* m_pMapObject = nullptr;
	CPickee* m_pPickeeCom = nullptr;
	_matrix m_IdentityMatrix;
	_uint m_uiToolIndex = 0;

	// 맵 툴 포인터 변수
	CMapToolDialog* m_pMapToolDialog = nullptr;
	// 오브젝트 툴 변수
	CObjectToolDialog* m_pObjectToolDialog = nullptr;
	// 네비메쉬 툴 변수
	CNaviMeshToolDialog* m_pNaviMeshToolDialog = nullptr;
	// 라이트 툴 변수
	CLightToolDialog* m_pLightToolDialog = nullptr;

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonMapLoad();

public:
	void SetPickingPos(_vec3& _vPos);
};


