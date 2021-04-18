#pragma once
#include "afxwin.h"


// CLightToolDialog ��ȭ �����Դϴ�.

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
	CLightToolDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLightToolDialog();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_LIGHTTOOLDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	// ����
	CToolView* m_pToolView = nullptr;
	// �Ŵ�����Ʈ
	CManagement* m_pManagement = nullptr;


	//���⼺ ���� Ŭ����
	CLight* m_pDirectionalLight = nullptr;

	// ���⼺ ������ ��ǻ��
	float m_fDirectionalDiffuse[RGBA_END];
	// ���⼺ ������ ����ŧ��
	float m_fDirectionalSpecular[RGBA_END];
	// ���⼺ ������ �����Ʈ
	float m_fDirectionalAmbient[RGBA_END];
	// ���⼺ ������ ����
	float m_fDirection[AXIS_END];

	// �� ������ ����Ʈ
	CListBox m_ListOfPointLight;

	// �� ������ ��ǻ��
	float m_fPointDiffuse[RGBA_END];
	// �� ������ ����ŧ��
	float m_fPointSpecular[RGBA_END];
	// �� ������ �����Ʈ
	float m_fPointAmbient[RGBA_END];
	// �� ������ ��ġ
	float m_fPointPosition[AXIS_END];
	// �� ������ ����
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