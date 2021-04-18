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

// CToolFormView �� ���Դϴ�.

class CToolFormView : public CFormView
{
	DECLARE_DYNCREATE(CToolFormView)

public:
	enum DIALOG_TYPE{ NAVI, OBJECT};

protected:
	CToolFormView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	// �� ������ ���� ���� �� ��Ʈ�� ����
	CTabCtrl m_TabControl;
	CToolStaticObject* m_pMapObject = nullptr;
	CPickee* m_pPickeeCom = nullptr;
	_matrix m_IdentityMatrix;
	_uint m_uiToolIndex = 0;

	// �� �� ������ ����
	CMapToolDialog* m_pMapToolDialog = nullptr;
	// ������Ʈ �� ����
	CObjectToolDialog* m_pObjectToolDialog = nullptr;
	// �׺�޽� �� ����
	CNaviMeshToolDialog* m_pNaviMeshToolDialog = nullptr;
	// ����Ʈ �� ����
	CLightToolDialog* m_pLightToolDialog = nullptr;

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonMapLoad();

public:
	void SetPickingPos(_vec3& _vPos);
};


