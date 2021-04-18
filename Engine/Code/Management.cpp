#include "..\Header\Management.h"
#include "..\Header\Scene.h"

// 싱글톤 구현
IMPLEMENT_SINGLETON(CManagement)

// 생성자
CManagement::CManagement()
	: m_pGraphicDevice(nullptr)
	, m_pInputDevice(nullptr)
	, m_pTargetManager(nullptr)
	, m_pOjectManager(nullptr)
	, m_pComponentManager(nullptr)
	, m_pCollisionManager(nullptr)
	, m_pTimerManager(nullptr)
	, m_pLightManager(nullptr)
	, m_pPipeline(nullptr)
	, m_pCurrentScene(nullptr){
}

// 엔진 초기화 작업을 진행하는 함수
HRESULT CManagement::InitEngine(_uint _uiSceneCount){
	// 그래픽 장치의 포인터 얻어오기
	m_pGraphicDevice = CGraphicDevice::GetInstance();
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;
	SafeAddRef(m_pGraphicDevice);

	// 입력 장치의 포인터 얻어오기
	m_pInputDevice = CInputDevice::GetInstance();
	if(nullptr == m_pInputDevice)
		return E_FAIL;
	SafeAddRef(m_pInputDevice);

	// 렌더 타겟 매니저의 포인터 얻어오기
	m_pTargetManager = CTargetManager::GetInstance();
	if(nullptr == m_pTargetManager)
		return E_FAIL;
	SafeAddRef(m_pTargetManager);

	// 오브젝트 매니저의 포인터 얻어오기
	m_pOjectManager = CObjectManager::GetInstance();
	if(nullptr == m_pOjectManager)
		return E_FAIL;
	SafeAddRef(m_pOjectManager);

	// 오브젝트 매니저에 클라이언트의 씬 갯수 전달
	if(FAILED(m_pOjectManager->ReserveLayerMapArray(_uiSceneCount)))
		return E_FAIL;

	// 컴포넌트 매니저의 포인터 얻어오기
	m_pComponentManager = CComponentManager::GetInstance();
	if(nullptr == m_pComponentManager)
		return E_FAIL;
	SafeAddRef(m_pComponentManager);

	// 컴포넌트 매니저에 클라이언트의 씬 갯수 전달
	if(FAILED(m_pComponentManager->ReserveComponentMapArray(_uiSceneCount)))
		return E_FAIL;

	m_pCollisionManager = CCollisionManager::GetInstance();
	if(nullptr == m_pCollisionManager)
		return E_FAIL;
	SafeAddRef(m_pCollisionManager);

	// 타이머 매니저의 포인터 얻어오기
	m_pTimerManager = CTimerManager::GetInstance();
	if(nullptr == m_pTimerManager)
		return E_FAIL;
	SafeAddRef(m_pTimerManager);

	// 라이트 매니저의 포인터 얻어오기
	m_pLightManager = CLightManager::GetInstance();
	if(nullptr == m_pLightManager)
		return E_FAIL;
	SafeAddRef(m_pLightManager);

	m_pSoundManager = CSoundManager::GetInstance();
	if(nullptr == m_pSoundManager)
		return E_FAIL;
	SafeAddRef(m_pSoundManager);

	// 파이프라인 객체의 포인터 얻어오기
	m_pPipeline = CPipeline::GetInstance();
	if(nullptr == m_pPipeline)
		return E_FAIL;
	SafeAddRef(m_pPipeline);

	return NOERROR;
}

//////////////////////////////////////////////////////////////////////////////////
/* 그래픽 장치 관련 */
// 그래픽디바이스 초기화 함수
HRESULT CManagement::InitGraphicDevice(HWND _hWnd, CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9 * _ppDevice/* = nullptr*/){
	if(nullptr == m_pGraphicDevice)
		return E_FAIL;

	if(FAILED(m_pGraphicDevice->InitGraphicDevice(_hWnd, _eMode, _uiWinCX, _uiWinCY, _ppDevice)))
		return E_FAIL;
	
	return NOERROR;
}

LPDIRECT3DDEVICE9 CManagement::GetGraphicDevice(){
	if(nullptr == m_pGraphicDevice)
		return nullptr;

	return m_pGraphicDevice->GetGraphicDevice();
}

//////////////////////////////////////////////////////////////////////////////////
/* 입력 장치 관련 */
// 입력 장치 초기화 함수
HRESULT CManagement::InitInputDevice(HINSTANCE _hInst, HWND _hWnd){
	if(nullptr == m_pInputDevice)
		return E_FAIL;
	
	return m_pInputDevice->InitInputDevice(_hInst, _hWnd);
}

// 키 입력 상태 업데이트
HRESULT CManagement::UpdateInputState(){
	if(nullptr == m_pInputDevice)
		return false;

	return m_pInputDevice->UpdateInputState();
}

// 입력한 키가 눌렸는지 검사
_bool CManagement::IsKeyPressing(_ubyte _DIKey){
	if(nullptr == m_pInputDevice)
		return false;

	return m_pInputDevice->IsKeyPressing(_DIKey);
}

// 키보드의 버튼이 눌린 직후만 참
_bool CManagement::IsKeyDown(_ubyte _DIKey) const{
	if(nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->IsKeyDown(_DIKey);
}

// 키보드의 버튼이 눌린 상태가 풀렸을때만 참
_bool CManagement::IsKeyUp(_ubyte _DIKey) const{
	if(nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->IsKeyUp(_DIKey);
}

// 마우스 버튼이 눌렸는지 검사
_bool CManagement::IsMouseButtonPressing(CInputDevice::MOUSE_BUTTON _eMouseButton){
	if(nullptr == m_pInputDevice)
		return false;

	return m_pInputDevice->IsMouseButtonPressing(_eMouseButton);
}

// 마우스의 버튼이 눌린 직후만 참
_bool CManagement::IsMouseButtonDown(CInputDevice::MOUSE_BUTTON _eMouseButton) const{
	if(nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->IsMouseButtonDown(_eMouseButton);
}

// 마우스의 버튼이 눌린 상태가 풀렸을때만 참
_bool CManagement::IsMouseButtonUp(CInputDevice::MOUSE_BUTTON _eMouseButton) const{
	if(nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->IsMouseButtonUp(_eMouseButton);
}

// 마우스의 움직임 정도를 가져옴
_long CManagement::GetMouseMovement(CInputDevice::MOUSE_MOVE _eMouseMove){
	if(nullptr == m_pInputDevice)
		return 0;

	return m_pInputDevice->GetMouseMovement(_eMouseMove);
}

HRESULT CManagement::SetttingRenderTargetOnShader(CShader* _pShader, const char* _szConstantTable, const _tchar* _szTargetKey){
	if(nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->SettingOnShader(_pShader, _szConstantTable, _szTargetKey);
}

//////////////////////////////////////////////////////////////////////////////////
/* 오브젝트 매니저 관련 */
// 오브젝트 매니저에 프로토타입 객체 전달
HRESULT CManagement::AddProtoObject(const _tchar * _szProtoKey, CGameObject* _pProtoObj){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	return m_pOjectManager->AddProtoObject(_szProtoKey, _pProtoObj);
}

// 프로토타입으로 오브젝트의 클론 객체 생성
HRESULT CManagement::AddCloneObjectToLayer(const _tchar * _szProtoKey, _uint _uiIndex, const _tchar * _szLayerKey, void * _pArgument/* = nullptr*/, CGameObject** _ppOutClone/* = nullptr*/){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	return m_pOjectManager->AddCloneObjectToLayer(_szProtoKey, _uiIndex, _szLayerKey, _pArgument, _ppOutClone);
}

HRESULT CManagement::ReleaseLayer(_uint _uiIndex, const _tchar * _szLayerKey){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	return m_pOjectManager->ReleaseLayer(_uiIndex, _szLayerKey);
}

HRESULT CManagement::ReleaseCloneObject(_uint _uiIndex, const _tchar * _szLayerKey, CGameObject * _pSelectedObj){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	return m_pOjectManager->ReleaseCloneObject(_uiIndex, _szLayerKey, _pSelectedObj);
}

// 씬에 설정된 오브젝트 삭제
HRESULT CManagement::SceneClear(_uint _uiIndex){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	return m_pOjectManager->SceneClear(_uiIndex);
}

HRESULT CManagement::UpdateObjectManager(_double _dDeltaTime){
	if(nullptr == m_pOjectManager)
		return E_FAIL;

	if(FAILED(m_pOjectManager->UpdateObjectMgr(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pOjectManager->LateUpdateObjectMgr(_dDeltaTime)))
		return E_FAIL;

	return NOERROR;
}

list<CGameObject*>* CManagement::GetObjectList(_uint _uiIndex, const _tchar* _szLayer){
	if(nullptr == m_pOjectManager)
		return nullptr;

	return m_pOjectManager->GetObjectList(_uiIndex, _szLayer);
}


//////////////////////////////////////////////////////////////////////////////////
/* 컴포넌트 매니저 관련 */
// 컴포넌트 매니저에 프로토타입 객체 전달
HRESULT CManagement::AddProtoComponent(const _tchar * _szProtoKey, _uint _uiIndex, CComponent * _pComponent){
	if(nullptr == m_pComponentManager)
		return E_FAIL;

	return 	m_pComponentManager->AddProtoComponent(_szProtoKey, _uiIndex, _pComponent);
}

// 프로토타입으로 컴포넌트의 클론 객체 생성
CComponent * CManagement::AddCloneComponent(const _tchar * _szProtoKey, _uint _uiIndex, void * _pArgument/* = nullptr*/){
	if(nullptr == m_pComponentManager)
		return nullptr;

	return m_pComponentManager->AddCloneComponent(_szProtoKey, _uiIndex, _pArgument);
}

HRESULT CManagement::ReleaseProtoComponent(const _tchar * _szKey, _uint _uiIndex){
	if(nullptr == m_pComponentManager)
		return E_FAIL;

	return m_pComponentManager->ReleaseProtoComponent(_szKey, _uiIndex);
}
//////////////////////////////////////////////////////////////////////////////////
/* 콜리전 매니저 관련 */
// 콜라이더 렌더
HRESULT CManagement::RenderCollider(){
	if(nullptr == m_pCollisionManager)
		return E_FAIL;

	return m_pCollisionManager->RenderCollider();
}

//////////////////////////////////////////////////////////////////////////////////
/* 라이트 매니저 관련 */
// 라이트 추가
HRESULT CManagement::AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc, CLight** _ppOutLight/* = nullptr*/){
	if(nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->AddLight(_pGraphicDevice, _pLightDesc, _ppOutLight);
}

const D3DLIGHT9* CManagement::GetLight(_uint _uiIndex){
	if(nullptr == m_pLightManager)
		return nullptr;

	return m_pLightManager->GetLight(_uiIndex);
}

HRESULT CManagement::ReleaseLight(CLight * _pLight){
	if(nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->ReleaseLight(_pLight);
}

// 라이트 초기화
HRESULT CManagement::ResetLight(){
	if(nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->ResetLight();
}

//////////////////////////////////////////////////////////////////////////////////
/* 타이머 매니저 관련 */
// 타이머 추가
HRESULT CManagement::AddTimer(const _tchar * _szTimerKey){
	if(nullptr == m_pTimerManager || nullptr == _szTimerKey)
		return E_FAIL;

	return m_pTimerManager->AddTimer(_szTimerKey);
}

// 타이머에서 시간 변화량 얻어옴
_double CManagement::ComputeDeltaTime(const _tchar * _szTimerKey){
	if(nullptr == m_pTimerManager || nullptr == _szTimerKey)
		return 0.0;

	return m_pTimerManager->ComputeDeltaTime(_szTimerKey);
}

//////////////////////////////////////////////////////////////////////////////////
/* 파이프라인 관련 */
// 파이프라인 객체에 저장된 변환 함수를 얻어오는 함수
HRESULT CManagement::GetTransform(D3DTRANSFORMSTATETYPE _eTransform, _matrix* _pMatrix){
	if(nullptr == m_pPipeline)
		return E_FAIL;

	return m_pPipeline->GetTransform(_eTransform, _pMatrix);
}

HRESULT CManagement::GetCameraWorldMatrix(_matrix * _vCamera){
	if(nullptr == m_pPipeline || nullptr == _vCamera)
		return E_FAIL;

	*_vCamera = m_pPipeline->GetCameraWorldMatrix();

	return NOERROR;
}

//////////////////////////////////////////////////////////////////////////////////
/* 씬 관련 */
// 씬을 받아와 현재 씬에 설정하는 함수
HRESULT CManagement::SetCurrentScene(CScene * _pScene){
	if(nullptr == _pScene)
		return E_FAIL;

	if(0 != SafeRelease(m_pCurrentScene)){
		MSG_BOX(L"m_pCurrentScene Release Failure");
		return E_FAIL;
	}

	m_pCurrentScene = _pScene;
	SafeAddRef(m_pCurrentScene);

	return NOERROR;
}

// Update Scene
HRESULT CManagement::UpdateCurrentScene(_double _dDeltaTime){
	if(nullptr == m_pCurrentScene ||
		nullptr == m_pOjectManager)
		return E_FAIL;

	if(FAILED(m_pCurrentScene->UpdateScene(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pOjectManager->UpdateObjectMgr(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pOjectManager->LateUpdateObjectMgr(_dDeltaTime)))
		return E_FAIL;

	if(FAILED(m_pCurrentScene->LateUpdateScene(_dDeltaTime)))
		return E_FAIL;

	return NOERROR;
}

// Render Scene
HRESULT CManagement::RenderCurrentScene(){
	if(nullptr == m_pCurrentScene)
		return E_FAIL;

	return m_pCurrentScene->RenderScene();
}

HRESULT CManagement::InitSound(){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->Initialize();

	return NOERROR;
}

HRESULT CManagement::UpdateSound(){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->UpdateSound();

	return NOERROR;
}

HRESULT CManagement::PlaySound(const _tchar * _szSoundKey, const _uint uiChannelID){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	if(uiChannelID > CSoundManager::MAX_CHANNEL)
		return E_FAIL;

	wstring wstrSoundKey(_szSoundKey);

	m_pSoundManager->PlaySound(wstrSoundKey, (CSoundManager::CHANNEL_ID)uiChannelID);

	return NOERROR;
}

HRESULT CManagement::StopSound(const _uint uiChannelID){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	if(uiChannelID > CSoundManager::MAX_CHANNEL)
		return E_FAIL;

	m_pSoundManager->StopSound((CSoundManager::CHANNEL_ID)uiChannelID);

	return NOERROR;
}

HRESULT CManagement::StopAll(){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	m_pSoundManager->StopAll();

	return NOERROR;
}

HRESULT CManagement::SetVolume(_uint uiChannelID, _float _fVol){
	if(nullptr == m_pSoundManager)
		return E_FAIL;

	if(uiChannelID > CSoundManager::MAX_CHANNEL)
		return E_FAIL;

	m_pSoundManager->SetVolume((CSoundManager::CHANNEL_ID)uiChannelID, _fVol);

	return NOERROR;
}

//////////////////////////////////////////////////////////////////////////////////

// Free
void CManagement::Free(){
	SafeRelease(m_pCurrentScene);
	SafeRelease(m_pSoundManager);
	SafeRelease(m_pOjectManager);
	SafeRelease(m_pComponentManager);
	SafeRelease(m_pCollisionManager);
	SafeRelease(m_pLightManager);
	SafeRelease(m_pTimerManager);
	SafeRelease(m_pPipeline);
	SafeRelease(m_pTargetManager);
	SafeRelease(m_pInputDevice);
	SafeRelease(m_pGraphicDevice);
}

//엔진 정리 작업을 진행하는 함수(static)
HRESULT CManagement::ReleaseEngine(){
	_ulong dwRefCount = 0;
	if(dwRefCount = CManagement::GetInstance()->DestroyInstance())
		MSG_BOX(L"CManagement Release Failure");

	if(dwRefCount = CObjectManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CObjectManager Release Failure");

	if(dwRefCount = CComponentManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CComponentManager Release Failure");

	if(dwRefCount = CCollisionManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CCollisionManager Release Failure");

	if(dwRefCount = CPickingManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CPickingManager Release Failure");

	if(dwRefCount = CLightManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CLightManager Release Failure");

	if(dwRefCount = CTimerManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CTimerManager Release Failure");

	if(dwRefCount = CPipeline::GetInstance()->DestroyInstance())
		MSG_BOX(L"CPipeline Release Failure");

	if(dwRefCount = CTargetManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CTargetManager Release Failure");

	if(dwRefCount = CInputDevice::GetInstance()->DestroyInstance())
		MSG_BOX(L"CInputDevice Release Failure");

	if(dwRefCount = CGraphicDevice::GetInstance()->DestroyInstance())
		MSG_BOX(L"CGraphicDevice Release Failure");

	if(dwRefCount = CSoundManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CSoundManager Release Failure");

	return NOERROR;
}
