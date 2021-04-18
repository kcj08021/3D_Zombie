
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "..\Header\MainFrm.h"
#include "..\Header\ToolDoc.h"
#include "..\Header\ToolView.h"
#include "..\Header\ToolTerrain.h"
#include "..\Header\ToolCamera.h"
#include "..\Header\ToolStaticObject.h"
#include "..\Header\ToolFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
	: m_pManagement(nullptr)
	, m_pGraphicDevice(nullptr)
	, m_pRendererCom(nullptr)
	, m_pToolTerrain(nullptr)
	, m_pToolCamera(nullptr)
	, m_pLight(nullptr)
	, m_pPickerCom(nullptr)
	, m_bIsInputStateUpdateFailure(false)
	, m_bIsObjManagerStateUpdateFailure(false)
	, m_bIsContinuallyMode(false){
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView(){
	SafeRelease(m_pToolTerrain);
	SafeRelease(m_pToolCamera);
	SafeRelease(m_pPickerCom);
	SafeRelease(m_pGraphicDevice);
	SafeRelease(m_pManagement);
	SafeRelease(m_pStaticMeshObject);
	SafeRelease(m_pRendererCom);

	CManagement::ReleaseEngine();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs){
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolViesw �׸���

void CToolView::OnDraw(CDC* /*pDC*/){
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(!pDoc)
		return;


	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.

	if(nullptr == m_pGraphicDevice)
		return;


	// �׷��� ����̽��� �ĸ���� ����
	m_pGraphicDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	// �ĸ���ۿ� �׸��� ����
	m_pGraphicDevice->BeginScene();

	// �������� ���� �׸���
	if(nullptr != m_pRendererCom)
		m_pRendererCom->DoRender(0.033);

	// �ĸ� ���ۿ� �׸��� ��
	m_pGraphicDevice->EndScene();
	// ���� �׷��� �ĸ� ���۸� ���� ���۷� ��ü �� �ÿ�
	m_pGraphicDevice->Present(nullptr, nullptr, 0, nullptr);
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const{
	CView::Dump(dc);
}

HRESULT CToolView::SaveHeightMap(const _tchar * _szPath){
	if(nullptr == m_pToolTerrain)
		return E_FAIL;

	return m_pToolTerrain->SaveHeightMap(_szPath);
}

void CToolView::SetTerrainShaderPass(_uint _uiShaderPass){
	if(nullptr == m_pToolTerrain)
		return;

	m_pToolTerrain->SetShaderPass(_uiShaderPass);
}

void CToolView::SetTerrainHeightValue(_uint _uiHeightValue){
	if(nullptr == m_pToolTerrain)
		return;

	m_pToolTerrain->SetHeightValue(_uiHeightValue);
}

void CToolView::SetTerrainBrushInfo(_uint _uiOuterSize, _uint _uiInnerSize){
	if(nullptr == m_pToolTerrain)
		return;

	m_pToolTerrain->SetBrushInfo(_uiOuterSize, _uiInnerSize);
}

void CToolView::SetCursorOption(_bool _bIsCursorRect){
	if(nullptr == m_pToolTerrain)
		return;

	m_pToolTerrain->SetBrushCursor(_bIsCursorRect);
}

POINT CToolView::GetMousePos(){
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	return POINT(pt);
}


CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate(){
	CView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	if(nullptr == pMainFrm){
		MSG_BOX(L"CToolView Initialize Failure");
		return;
	}

	RECT rcMainFrm = {};
	//GetWindowRect()�Լ��� �������� �׵θ��� ������ ��üũ�⸦ ������ �Լ�.
	//������ RECT�� ��ǥ�� ��ũ�� ������ ��ǥ�̴�.
	pMainFrm->GetWindowRect(&rcMainFrm);

	//�簢�� ���
	::SetRect(&rcMainFrm, 0, 0, rcMainFrm.right - rcMainFrm.left, rcMainFrm.bottom - rcMainFrm.top);

	RECT rcView = {};
	//GetClientRect()�Լ��� �׵θ� �������� ������ ���� Ŭ���̾�Ʈ�� ũ�⸦ ������ �Լ�.
	//�� �Լ��� ������ RECT�� ��ǥ�� Ŭ���̾�Ʈ ��ǥ(left�� top�� �׻� 0)
	GetClientRect(&rcView);

	int iGapX = rcMainFrm.right - rcView.right;
	int iGapY = rcMainFrm.bottom - rcView.bottom;

	//SetWindowPos()�Լ��� MainFrm�� ũ�⸦ �����ϴ� �Լ�
	pMainFrm->SetWindowPos(nullptr, 50, 50, WINCX + iGapX, WINCY + iGapY, SWP_NOZORDER);

	m_pManagement = CManagement::GetInstance();
	if(nullptr == m_pManagement)
		return;
	SafeAddRef(m_pManagement);

	m_pManagement->InitEngine(1);

	if(FAILED(m_pManagement->InitGraphicDevice(m_hWnd, CGraphicDevice::MODE_WINDOWED, WINCX, WINCY, &m_pGraphicDevice))){
		MSG_BOX(L"GraphicDevice Initialize Failure");
		return;
	}

	if(FAILED(m_pManagement->InitInputDevice(::AfxGetApp()->m_hInstance, ::AfxGetApp()->GetMainWnd()->m_hWnd))){
		MSG_BOX(L"InputDevice Initialize Failure");
		return;
	}

	// ������Ʈ���� ������Ÿ�� ���
	if(FAILED(AddRenderComponent())){
		return;
	}
	if(FAILED(AddShaderComponent())){
		return;
	}
	if(FAILED(AddTextureComponent())){
		return;
	}
	if(FAILED(AddTransformComponent())){
		return;
	}
	if(FAILED(AddPickingComponent())){
		return;
	}
	if(FAILED(AddColliderComponent()))
		return;

	if(FAILED(CreateToolCamera())){
		return;
	}

	if(FAILED(CreateToolTerrain())){
		return;
	}

	if(FAILED(CreateToolStaticMeshObeject()))
		return;

	if(FAILED(CreateNaviMeshObject()))
		return;

	//if(FAILED(CreateLight()))
	//	return;


	SetTimer(0, 33, NULL);

	// 60������ ������ Ÿ�̸� ����
	if(FAILED(m_pManagement->AddTimer(L"30FpsTimer")))
		MSG_BOX(L"30FpsTimer Create Failure");
}

void CToolView::OnTimer(UINT_PTR nIDEvent){
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	_double dDeltaTime = m_pManagement->ComputeDeltaTime(L"30FpsTimer");

	switch(nIDEvent){
	case 0:
		// �Է� ���� ������Ʈ 
		if(FAILED(m_pManagement->UpdateInputState()))
			if(!m_bIsInputStateUpdateFailure){
				MSG_BOX(L"CInputDevice Update Failure");
				m_bIsInputStateUpdateFailure = true;
			}


		// �Ŵ�����Ʈ���� �����ǰ� �ִ� ������Ʈ �Ŵ����� ������Ʈ
		if(FAILED(m_pManagement->UpdateObjectManager(dDeltaTime))){
			if(!m_bIsObjManagerStateUpdateFailure){
				MSG_BOX(L"CObjectManager Update Failure");
				m_bIsInputStateUpdateFailure = true;
			}
		}

		if(m_pManagement->IsKeyDown(DIK_F2))
			m_pRendererCom->SetRenderDebugBuffer(!m_pRendererCom->GetRenderDebugBuffer());

		if(m_pManagement->IsMouseButtonDown(CInputDevice::MOUSE_LBUTTON)){
			POINT pt = {};
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			if(pt.x > 0 && pt.x < 960
				&& pt.y > 0  && pt.y < 720){
				CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
				if(nullptr == pMainFrm){
					MSG_BOX(L"CToolView Initialize Failure");
					return;
				}
				_vec3 vPos = {};
				if(m_pPickerCom->CheckPicking(pt, &vPos)){
					pMainFrm->GetFormView()->SetPickingPos(vPos);
				}
			}
			break;
		}
	default:
		break;
	}
	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}


// ������ ������Ʈ�� ����ϴ� �Լ�
HRESULT CToolView::AddRenderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"RendererCom_Proto", 0, m_pRendererCom = CRenderer::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

// ���̴� ������Ʈ�� ����ϴ� �Լ�
HRESULT CToolView::AddShaderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"DefaultShaderCom_Proto", 0, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/DefaultShader.fx"))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"TerrainShaderCom_Proto", 0, CShader::CreateProto(m_pGraphicDevice, L"../Bin/ShaderFile/TerrainShader.fx"))))
		return E_FAIL;

	return NOERROR;
}

// �ؽ��ĸ� �ε��Ͽ� ������Ʈ�� ����ϴ� �Լ�
HRESULT CToolView::AddTextureComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	//if(FAILED(m_pManagement->AddProtoComponent(L"TextureCom_Proto", 0, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../Bin/Resourece/Texture/Default.png"))))
	//	return E_FAIL;

	//if(FAILED(m_pManagement->AddProtoComponent(L"TerrainTextureCom_Proto", 0, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../Bin/Resourece/Texture/Terrain/Grass_%d.tga", 2))))
	//	return E_FAIL;

	//if(FAILED(m_pManagement->AddProtoComponent(L"FilterTextureCom_Proto", 0, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../Bin/Resourece/Texture/Terrain/Filter.bmp"))))
	//	return E_FAIL;

	//if(FAILED(m_pManagement->AddProtoComponent(L"BrushTextureCom_Proto", 0, CTexture::CreateProto(m_pGraphicDevice, CTexture::TEXTURE_GENERAL, L"../Bin/Resourece/Texture/Terrain/Brush.png"))))
	//	return E_FAIL;

	return NOERROR;
}

// CTransform ������Ʈ�� ����ϴ� �Լ�
HRESULT CToolView::AddTransformComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"TransformCom_Proto", 0, CTransform::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolView::AddPickingComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"PickerCom_Proto", 0, CPicker::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	m_pPickerCom = dynamic_cast<CPicker*>(m_pManagement->AddCloneComponent(L"PickerCom_Proto", 0));
	if(nullptr == m_pPickerCom){
		return E_FAIL;
	}

	if(FAILED(m_pManagement->AddProtoComponent(L"PickeeCom_Proto", 0, CPickee::CreateProto(m_pGraphicDevice))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolView::AddColliderComponent(){
	if(nullptr == m_pManagement)
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Collider_AABB_Proto", 0, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_AABB))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Collider_OBB_Proto", 0, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_OBB))))
		return E_FAIL;

	if(FAILED(m_pManagement->AddProtoComponent(L"Collider_Shpere_Proto", 0, CCollider::CreateProto(m_pGraphicDevice, CCollider::COLLTYPE_SPHERE))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CToolView::CreateToolCamera(){
	m_pManagement->AddProtoObject(L"ToolCamera_Proto", CToolCamera::CreateProto(m_pGraphicDevice));

	CCamera::CAMERA_DESC tCameraDesc;
	tCameraDesc.vEye = _vec3(0.f, 10.f, -10.f);
	tCameraDesc.vAt = _vec3(0.f, 0.f, 0.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);
	tCameraDesc.fFovY = D3DXToRadian(60.f);
	tCameraDesc.fAspect = 4.f / 3.f;
	tCameraDesc.fFar = 500.f;
	tCameraDesc.fNear = 0.3f;
	tCameraDesc.tTransformDesc = { nullptr, 10.f, D3DXToRadian(90.f) };

	if(FAILED(m_pManagement->AddCloneObjectToLayer(L"ToolCamera_Proto", 0, L"Camera", &tCameraDesc, reinterpret_cast<CGameObject**>(&m_pToolCamera)))){
		return E_FAIL;
	}
	SafeAddRef(m_pToolCamera);

	return NOERROR;
}

HRESULT CToolView::CreateToolTerrain(){
	if(FAILED(m_pManagement->AddProtoObject(L"Terrain_Proto", CToolTerrain::CreateProto(m_pGraphicDevice)))){
		return E_FAIL;
	}

	if(FAILED(m_pManagement->AddCloneObjectToLayer(L"Terrain_Proto", 0, L"Terrain_Layer", nullptr, reinterpret_cast<CGameObject**>(&m_pToolTerrain)))){
		return E_FAIL;
	}
	SafeAddRef(m_pToolTerrain);

	return NOERROR;
}

HRESULT CToolView::CreateToolStaticMeshObeject(){
	m_pManagement->AddProtoObject(L"StaticMesh_Proto", CToolStaticObject::CreateProto(m_pGraphicDevice));

	return NOERROR;
}

HRESULT CToolView::CreateNaviMeshObject(){

	return NOERROR;
}

HRESULT CToolView::CreateLight(){

	D3DLIGHT9 LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Direction = _vec3(1.f, -1.f, 1.f);

	if(FAILED(m_pManagement->AddLight(m_pGraphicDevice, &LightDesc)))
		return E_FAIL;

	return NOERROR;
}
