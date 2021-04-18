
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once

#include "ToolDoc.h"

// Engine 헤더
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
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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
	// 지형 셰이더의 패스 설정(WireFrame 혹은 Solid)
	void SetTerrainShaderPass(_uint _uiShaderPass);
	// 지형 높이 편집시 변경되는 높이 설정
	void SetTerrainHeightValue(_uint _uiHeightValue);
	// 지형 높이 편집시 브러쉬의 정보 설정
	void SetTerrainBrushInfo(_uint _uiOuterSize, _uint _uiInnerSize);
	// 지형 편집시 클릭옵션 설정
	void SetClickOption(_bool _bIsContinuallyMode){
		m_bIsContinuallyMode = _bIsContinuallyMode;
	}
	// 커서 모양 설정
	void SetCursorOption(_bool _bIsCursorRect);

	void SetStaticMeshMap(CToolStaticObject* _pStaticMeshObject){
		m_pStaticMeshObject = _pStaticMeshObject;
	}
	void SetNaviMeshObject(CNaviMeshObject* _pNaviMeshObject){
		m_pNaviMeshObject = _pNaviMeshObject;
	}

private:
	// 매니지먼트
	CManagement* m_pManagement = nullptr;
	// 그래픽 장치
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 렌더러
	CRenderer* m_pRendererCom = nullptr;

private:
	// 지형
	CToolTerrain* m_pToolTerrain = nullptr;
	// 스태틱 메쉬 통맵
	CToolStaticObject* m_pStaticMeshObject = nullptr;
	// 네비 메쉬 오브젝트
	CNaviMeshObject* m_pNaviMeshObject = nullptr;
	// 카메라
	CToolCamera* m_pToolCamera = nullptr;
	// 조명
	CLight* m_pLight = nullptr;

	// 피커 컴포넌트
	CPicker* m_pPickerCom = nullptr;

	// 입력 장치 상태 업데이트 실패시 메세지 확인 후 true
	_bool m_bIsInputStateUpdateFailure = false;
	// 오브젝트 매니저 상태 업데이트 실패시 메세지 확인 후 true
	_bool m_bIsObjManagerStateUpdateFailure = false;
	// 지형 편집시 연속적으로 올라가는 형태로 만드는지 여부
	_bool m_bIsContinuallyMode = false;

	// 컴포넌트 추가 관련
private:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 렌더러 컴포넌트를 등록하는 함수
	HRESULT AddRenderComponent();
	// 셰이더 컴포넌트를 등록하는 함수
	HRESULT AddShaderComponent();
	// 텍스쳐를 로드하여 컴포넌트로 등록하는 함수
	HRESULT AddTextureComponent();
	// CTransform 컴 포넌트를 등록하는 함수
	HRESULT AddTransformComponent();
	// 피킹을 위한 컴포넌트를 등록하는 함수
	HRESULT AddPickingComponent();
	// 콜라이더 컴포넌트 등록
	HRESULT AddColliderComponent();


	// 카메라를 생성하는 함수
	HRESULT CreateToolCamera();
	// 지형을 생성하는 함수
	HRESULT CreateToolTerrain();
	// 스태틱 메쉬 오브젝트 추가하는 함수
	HRESULT CreateToolStaticMeshObeject();
	// 네비 메쉬 컴포넌트 등록
	HRESULT CreateNaviMeshObject();

	HRESULT CreateLight();
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

