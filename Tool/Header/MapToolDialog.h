#pragma once

#include "Management.h"

class CToolView;

BEGIN(Tool)
class CToolTerrain;
END

USING(Tool)

// CMapToolDialog ��ȭ �����Դϴ�.

class CMapToolDialog : public CDialog
{
	DECLARE_DYNAMIC(CMapToolDialog)

public:
	CMapToolDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapToolDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSizeApply();
	afx_msg void OnBnClickedRenderOption(UINT msg);
	afx_msg void OnEnChangedHetghtRangeValue(UINT msg);
	afx_msg void OnBnClickedRadioClickOption(UINT msg);
	afx_msg void OnBnClickedRadioCursorOption(UINT msg);

private:
	// ������ ������ ����
	CToolView* m_pToolView = nullptr;

	// ������ X ũ��
	_uint m_uiSizeX = 0;
	// ������ Z ũ��
	_uint m_uiSizeZ = 0;
	// ������ ���� ���� ����
	_float m_fInterval = 0.f;

	// ���̴� �н� ����(WireFrame Ȥ�� Solid)
	int m_iShaderPass = 0;

	// Ŭ���� ��µǴ� ������ ���� ��
	_uint m_uiHeightValue = 0;
	// Ŭ���� ���̰��� ������ ��ġ�� �ܺ� ����
	_uint m_uiOuterRange = 0;
	// Ŭ���� ���̰����� ��µǴ� ���� ����
	_uint m_uiInnerRange = 0;
public:
	afx_msg void OnBnClickedButtonHeightSave();
	afx_msg void OnBnClickedButtonHeightLoad();
};
