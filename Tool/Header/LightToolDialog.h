#pragma once
#include "afxwin.h"


// CLightToolDialog 대화 상자입니다.

BEGIN(Engine)
class CManagement;
class CLight;
END

class CToolView;
class CLightToolDialog: public CDialog{
	DECLARE_DYNAMIC(CLightToolDialog)

private:
	enum{ R = 0, G, B, A, RGBA_END };
	enum{ X = 0, Y, Z, AXIS_END };

public:
	CLightToolDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightToolDialog();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_LIGHTTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	// 툴뷰
	CToolView* m_pToolView = nullptr;
	// 매니지먼트
	CManagement* m_pManagement = nullptr;


	//방향성 조명 클래스
	CLight* m_pDirectionalLight = nullptr;

	// 방향성 조명의 디퓨즈
	float m_fDirectionalDiffuse[RGBA_END];
	// 방향성 조명의 스펙큘러
	float m_fDirectionalSpecular[RGBA_END];
	// 방향성 조명의 엠비언트
	float m_fDirectionalAmbient[RGBA_END];
	// 방향성 조명의 방향
	float m_fDirection[AXIS_END];

	// 점 조명의 리스트
	CListBox m_ListOfPointLight;

	// 점 조명의 디퓨즈
	float m_fPointDiffuse[RGBA_END];
	// 점 조명의 스펙큘러
	float m_fPointSpecular[RGBA_END];
	// 점 조명의 엠비언트
	float m_fPointAmbient[RGBA_END];
	// 점 조명의 위치
	float m_fPointPosition[AXIS_END];
	// 점 조명의 범위
	float m_fPointRange;
public:
	virtual BOOL OnInitDialog();

public:
	void SetPickingPos(_vec3 _vPos);

private:
	afx_msg void OnBnClickedButtonDirectionApply();
	afx_msg void OnBnClickedButtonPointAdd();
	afx_msg void OnBnClickedButtonPointApply();
	afx_msg void OnBnClickedButtonPointDelete();
	afx_msg void OnBnClickedButtonLightSave();
	afx_msg void OnBnClickedButtonLightLoad();
	afx_msg void OnLbnSelchangeList1();

};