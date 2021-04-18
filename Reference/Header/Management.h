#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#include "Base.h"
#include "..\Header\GraphicDevice.h"
#include "..\Header\InputDevice.h"
#include "..\Header\TargetManager.h"
#include "..\Header\TimerManager.h"
#include "..\Header\ObjectManager.h"
#include "..\Header\ComponentManager.h"
#include "..\Header\PickingManager.h"
#include "..\Header\CollisionManager.h"
#include "..\Header\LightManager.h"
#include "..\Header\SoundManager.h"
#include "..\Header\Pipeline.h"

BEGIN(Engine)
// 전방 선언
class CScene;

class ENGINE_DLL CManagement final: public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CManagement)

private:
	// 생성자
	explicit CManagement();
	// 소멸자
	virtual ~CManagement() = default;

public:
	//엔진 초기화 작업을 진행하는 함수
	HRESULT InitEngine(_uint _uiSceneCount);

public:	/* 그래픽 장치 관련 */
	// 그래픽 장치 초기화 함수
	HRESULT InitGraphicDevice(HWND _hWnd, CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice = nullptr);
	// 그래픽 장치 변수 가져오기
	LPDIRECT3DDEVICE9 GetGraphicDevice();

public:	/* 입력 장치 관련 */
	// 입력 장치 초기화 함수
	HRESULT InitInputDevice(HINSTANCE _hInst, HWND _hWnd);
	// 입력 상태 업데이트
	HRESULT UpdateInputState();
	// 입력한 키가 눌렸는지 검사
	_bool IsKeyPressing(_ubyte _DIKey);
	// 키보드의 버튼이 눌린 직후만 참
	_bool IsKeyDown(_ubyte _DIKey) const;
	// 키보드의 버튼이 눌린 상태가 풀렸을때만 참
	_bool IsKeyUp(_ubyte _DIKey) const;
	// 마우스 버튼이 눌렸는지 검사
	_bool IsMouseButtonPressing(CInputDevice::MOUSE_BUTTON _eMouseButton);
	// 마우스의 버튼이 눌린 직후만 참
	_bool IsMouseButtonDown(CInputDevice::MOUSE_BUTTON _eMouseButton) const;
	// 마우스의 버튼이 눌린 상태가 풀렸을때만 참
	_bool IsMouseButtonUp(CInputDevice::MOUSE_BUTTON _eMouseButton) const;
	// 마우스의 움직임 정도를 가져옴
	_long GetMouseMovement(CInputDevice::MOUSE_MOVE _eMouseMove);

public: /* 렌더타겟 매니저 관련 */
	// 셰이더의 전역변수에 렌더타겟을 설정
	HRESULT SetttingRenderTargetOnShader(CShader* _pShader, const char* _szConstantTable, const _tchar* _szTargetKey);

public:	/* 오브젝트 매니저 관련 */
	// 오브젝트 매니저에 프로토타입 객체 전달
	HRESULT AddProtoObject(const _tchar* _szProtoKey, CGameObject* _pProtoObj);
	// 프로토타입으로 클론 객체 생성
	HRESULT AddCloneObjectToLayer(const _tchar* _szProtoKey, _uint _uiIndex, const _tchar* _szLayerKey, void* _pArgument = nullptr, CGameObject** _ppOutClone = nullptr);
	// 레이어 삭제
	HRESULT ReleaseLayer(_uint _uiIndex, const _tchar* _szLayerKey);
	// 레이어 안의 복제 오브젝트 삭제
	HRESULT ReleaseCloneObject(_uint _uiIndex, const _tchar* _szLayerKey, CGameObject* _pSelectedObj);
	// 씬에 설정된 오브젝트 삭제
	HRESULT SceneClear(_uint _uiIndex);
	// Update ObjMgr
	HRESULT UpdateObjectManager(_double _dDeltaTime);
	// 오브젝트 레이어 가져옴
	list<CGameObject*>* GetObjectList(_uint _uiIndex, const _tchar* _szLayer);

public:	/* 컴포넌트 매니저 관련 */
	// 컴포넌트 매니저에 프로토타입 객체 전달
	HRESULT AddProtoComponent(const _tchar* _szProtoKey, _uint _uiIndex, CComponent* _pComponent);
	// 프로토타입으로 클론 객체 생성
	CComponent* AddCloneComponent(const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument = nullptr);
	// 특정 프로토타입 컴포넌트를 삭제함
	HRESULT ReleaseProtoComponent(const _tchar * _szKey, _uint _uiIndex);

public: /* 콜리전 매니저 관련 */
	HRESULT RenderCollider();

public: /* 라이트 매니저 관련 */
	// 라이트 추가
	HRESULT AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc, CLight** _ppOutLight = nullptr);
	// 라이트 얻어오기
	const D3DLIGHT9* GetLight(_uint _uiIndex);
	// 라이트 삭제
	HRESULT ReleaseLight(CLight* _pLight);
	// 라이트 초기화
	HRESULT ResetLight();

public:	/* 타이머 매니저 관련 */
	// 타이머 추가
	HRESULT AddTimer(const _tchar* _szTimerKey);
	// 타이머에서 시간 변화량 얻어옴
	_double ComputeDeltaTime(const _tchar* _szTimerKey);

public:	/* 파이프라인 관련 */
	// 파이프라인 객체에 저장된 변환 함수를 얻어오는 함수
	HRESULT GetTransform(D3DTRANSFORMSTATETYPE _eTransform, _matrix* _pMatrix);
	// 카메라의 월드행렬을 가져오는 함수
	HRESULT GetCameraWorldMatrix(_matrix* _vCameraDirection);

public:	/* 씬 관련 */	
	// 씬을 받아와 현재 씬에 설정하는 함수
	HRESULT SetCurrentScene(CScene* _pScene);
	// Update Scene
	HRESULT UpdateCurrentScene(_double _dDeltaTime);
	// Render Scene
	HRESULT RenderCurrentScene();

public: /* 사운드 관련 */
	// 사운드 준비
	HRESULT InitSound();
	// 업데이트
	HRESULT UpdateSound();
	// 채널에 사운드 재생
	HRESULT PlaySound(const _tchar* _szSoundKey, const _uint uiChannelID);
	// 특정 채널의 사운드 정지
	HRESULT StopSound(const _uint uiChannelID);
	// 모든 사운드 정지
	HRESULT StopAll();
	// 볼륨 설정
	HRESULT SetVolume(_uint uiChannelID, _float _fVol);

private:
	// 그래픽 장치
	CGraphicDevice* m_pGraphicDevice = nullptr;
	// 입력 장치
	CInputDevice* m_pInputDevice = nullptr;
	// 렌더타겟 매니저
	CTargetManager* m_pTargetManager = nullptr;
	// 오브젝트 매니저
	CObjectManager* m_pOjectManager = nullptr;
	// 컴포넌트 매니저
	CComponentManager* m_pComponentManager = nullptr;
	// 콜리전 매니저
	CCollisionManager* m_pCollisionManager = nullptr;
	// 타이머 매니저
	CTimerManager* m_pTimerManager = nullptr;
	// 라이트 매니저
	CLightManager* m_pLightManager = nullptr;
	// 사운드 매니저
	CSoundManager* m_pSoundManager = nullptr;
	// 파이프라인(뷰 변환 행렬과 투영 행렬을 가짐)
	CPipeline* m_pPipeline = nullptr;
	// 현재 씬
	CScene* m_pCurrentScene	= nullptr;

public:
	// Free
	virtual void Free() override;

	// static 함수
public:
	//엔진 정리 작업을 진행하는 함수(static)
	static HRESULT ReleaseEngine();
};

END

#endif // !__MANAGEMENT_H__
