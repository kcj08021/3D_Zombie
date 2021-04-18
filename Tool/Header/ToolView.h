
// ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once

#include "ToolDoc.h"

// Engine ���
#include "Management.h"

BEGIN(Tool)
class CToolTerrain;
class CToolCamera;
class CToolStaticObject;
class CNaviMeshObject;
END

USING(Tool);

class  CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	CToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

public:
	HRESULT SaveHeightMap(const _tchar* _szPath);

	// Get
public:
	LPDIRECT3DDEVICE9 GetDevice(){
		return m_pGraphicDevice;
	}
	CToolTerrain* GetToolTerrain(){
		return m_pToolTerrain;
	}
	CToolCamera* GetCamera(){
		return m_pToolCamera;
	}
	POINT GetMousePos();
	
	// Set
public:
	// ���� ���̴��� �н� ����(WireFrame Ȥ�� Solid)
	void SetTerrainShaderPass(_uint _uiShaderPass);
	// ���� ���� ������ ����Ǵ� ���� ����
	void SetTerrainHeightValue(_uint _uiHeightValue);
	// ���� ���� ������ �귯���� ���� ����
	void SetTerrainBrushInfo(_uint _uiOuterSize, _uint _uiInnerSize);
	// ���� ������ Ŭ���ɼ� ����
	void SetClickOption(_bool _bIsContinuallyMode){
		m_bIsContinuallyMode = _bIsContinuallyMode;
	}
	// Ŀ�� ��� ����
	void SetCursorOption(_bool _bIsCursorRect);

	void SetStaticMeshMap(CToolStaticObject* _pStaticMeshObject){
		m_pStaticMeshObject = _pStaticMeshObject;
	}
	void SetNaviMeshObject(CNaviMeshObject* _pNaviMeshObject){
		m_pNaviMeshObject = _pNaviMeshObject;
	}

private:
	// �Ŵ�����Ʈ
	CManagement* m_pManagement = nullptr;
	// �׷��� ��ġ
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// ������
	CRenderer* m_pRendererCom = nullptr;

private:
	// ����
	CToolTerrain* m_pToolTerrain = nullptr;
	// ����ƽ �޽� ���
	CToolStaticObject* m_pStaticMeshObject = nullptr;
	// �׺� �޽� ������Ʈ
	CNaviMeshObject* m_pNaviMeshObject = nullptr;
	// ī�޶�
	CToolCamera* m_pToolCamera = nullptr;
	// ����
	CLight* m_pLight = nullptr;

	// ��Ŀ ������Ʈ
	CPicker* m_pPickerCom = nullptr;

	// �Է� ��ġ ���� ������Ʈ ���н� �޼��� Ȯ�� �� true
	_bool m_bIsInputStateUpdateFailure = false;
	// ������Ʈ �Ŵ��� ���� ������Ʈ ���н� �޼��� Ȯ�� �� true
	_bool m_bIsObjManagerStateUpdateFailure = false;
	// ���� ������ ���������� �ö󰡴� ���·� ������� ����
	_bool m_bIsContinuallyMode = false;

	// ������Ʈ �߰� ����
private:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// ������ ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddRenderComponent();
	// ���̴� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddShaderComponent();
	// �ؽ��ĸ� �ε��Ͽ� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddTextureComponent();
	// CTransform �� ����Ʈ�� ����ϴ� �Լ�
	HRESULT AddTransformComponent();
	// ��ŷ�� ���� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddPickingComponent();
	// �ݶ��̴� ������Ʈ ���
	HRESULT AddColliderComponent();


	// ī�޶� �����ϴ� �Լ�
	HRESULT CreateToolCamera();
	// ������ �����ϴ� �Լ�
	HRESULT CreateToolTerrain();
	// ����ƽ �޽� ������Ʈ �߰��ϴ� �Լ�
	HRESULT CreateToolStaticMeshObeject();
	// �׺� �޽� ������Ʈ ���
	HRESULT CreateNaviMeshObject();

	HRESULT CreateLight();
};

#ifndef _DEBUG  // ToolView.cpp�� ����� ����
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

