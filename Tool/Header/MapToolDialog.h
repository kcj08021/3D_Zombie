#pragma once

#include "Management.h"

class CToolView;

BEGIN(Tool)
class CToolTerrain;
END

USING(Tool)

// CMapToolDialog 대화 상자입니다.

class CMapToolDialog : public CDialog
{
	DECLARE_DYNAMIC(CMapToolDialog)

public:
	CMapToolDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapToolDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSizeApply();
	afx_msg void OnBnClickedRenderOption(UINT msg);
	afx_msg void OnEnChangedHetghtRangeValue(UINT msg);
	afx_msg void OnBnClickedRadioClickOption(UINT msg);
	afx_msg void OnBnClickedRadioCursorOption(UINT msg);

private:
	// 툴뷰의 포인터 변수
	CToolView* m_pToolView = nullptr;

	// 지형의 X 크기
	_uint m_uiSizeX = 0;
	// 지형의 Z 크기
	_uint m_uiSizeZ = 0;
	// 지형의 간격 설정 변수
	_float m_fInterval = 0.f;

	// 셰이더 패스 변수(WireFrame 혹은 Solid)
	int m_iShaderPass = 0;

	// 클릭시 상승되는 지형의 높이 값
	_uint m_uiHeightValue = 0;
	// 클릭시 높이값이 영향을 끼치는 외부 범위
	_uint m_uiOuterRange = 0;
	// 클릭시 높이값으로 상승되는 내부 범위
	_uint m_uiInnerRange = 0;
public:
	afx_msg void OnBnClickedButtonHeightSave();
	afx_msg void OnBnClickedButtonHeightLoad();
};
