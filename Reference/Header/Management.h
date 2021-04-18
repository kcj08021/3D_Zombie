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
// ���� ����
class CScene;

class ENGINE_DLL CManagement final: public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CManagement)

private:
	// ������
	explicit CManagement();
	// �Ҹ���
	virtual ~CManagement() = default;

public:
	//���� �ʱ�ȭ �۾��� �����ϴ� �Լ�
	HRESULT InitEngine(_uint _uiSceneCount);

public:	/* �׷��� ��ġ ���� */
	// �׷��� ��ġ �ʱ�ȭ �Լ�
	HRESULT InitGraphicDevice(HWND _hWnd, CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY, LPDIRECT3DDEVICE9* _ppDevice = nullptr);
	// �׷��� ��ġ ���� ��������
	LPDIRECT3DDEVICE9 GetGraphicDevice();

public:	/* �Է� ��ġ ���� */
	// �Է� ��ġ �ʱ�ȭ �Լ�
	HRESULT InitInputDevice(HINSTANCE _hInst, HWND _hWnd);
	// �Է� ���� ������Ʈ
	HRESULT UpdateInputState();
	// �Է��� Ű�� ���ȴ��� �˻�
	_bool IsKeyPressing(_ubyte _DIKey);
	// Ű������ ��ư�� ���� ���ĸ� ��
	_bool IsKeyDown(_ubyte _DIKey) const;
	// Ű������ ��ư�� ���� ���°� Ǯ�������� ��
	_bool IsKeyUp(_ubyte _DIKey) const;
	// ���콺 ��ư�� ���ȴ��� �˻�
	_bool IsMouseButtonPressing(CInputDevice::MOUSE_BUTTON _eMouseButton);
	// ���콺�� ��ư�� ���� ���ĸ� ��
	_bool IsMouseButtonDown(CInputDevice::MOUSE_BUTTON _eMouseButton) const;
	// ���콺�� ��ư�� ���� ���°� Ǯ�������� ��
	_bool IsMouseButtonUp(CInputDevice::MOUSE_BUTTON _eMouseButton) const;
	// ���콺�� ������ ������ ������
	_long GetMouseMovement(CInputDevice::MOUSE_MOVE _eMouseMove);

public: /* ����Ÿ�� �Ŵ��� ���� */
	// ���̴��� ���������� ����Ÿ���� ����
	HRESULT SetttingRenderTargetOnShader(CShader* _pShader, const char* _szConstantTable, const _tchar* _szTargetKey);

public:	/* ������Ʈ �Ŵ��� ���� */
	// ������Ʈ �Ŵ����� ������Ÿ�� ��ü ����
	HRESULT AddProtoObject(const _tchar* _szProtoKey, CGameObject* _pProtoObj);
	// ������Ÿ������ Ŭ�� ��ü ����
	HRESULT AddCloneObjectToLayer(const _tchar* _szProtoKey, _uint _uiIndex, const _tchar* _szLayerKey, void* _pArgument = nullptr, CGameObject** _ppOutClone = nullptr);
	// ���̾� ����
	HRESULT ReleaseLayer(_uint _uiIndex, const _tchar* _szLayerKey);
	// ���̾� ���� ���� ������Ʈ ����
	HRESULT ReleaseCloneObject(_uint _uiIndex, const _tchar* _szLayerKey, CGameObject* _pSelectedObj);
	// ���� ������ ������Ʈ ����
	HRESULT SceneClear(_uint _uiIndex);
	// Update ObjMgr
	HRESULT UpdateObjectManager(_double _dDeltaTime);
	// ������Ʈ ���̾� ������
	list<CGameObject*>* GetObjectList(_uint _uiIndex, const _tchar* _szLayer);

public:	/* ������Ʈ �Ŵ��� ���� */
	// ������Ʈ �Ŵ����� ������Ÿ�� ��ü ����
	HRESULT AddProtoComponent(const _tchar* _szProtoKey, _uint _uiIndex, CComponent* _pComponent);
	// ������Ÿ������ Ŭ�� ��ü ����
	CComponent* AddCloneComponent(const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument = nullptr);
	// Ư�� ������Ÿ�� ������Ʈ�� ������
	HRESULT ReleaseProtoComponent(const _tchar * _szKey, _uint _uiIndex);

public: /* �ݸ��� �Ŵ��� ���� */
	HRESULT RenderCollider();

public: /* ����Ʈ �Ŵ��� ���� */
	// ����Ʈ �߰�
	HRESULT AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc, CLight** _ppOutLight = nullptr);
	// ����Ʈ ������
	const D3DLIGHT9* GetLight(_uint _uiIndex);
	// ����Ʈ ����
	HRESULT ReleaseLight(CLight* _pLight);
	// ����Ʈ �ʱ�ȭ
	HRESULT ResetLight();

public:	/* Ÿ�̸� �Ŵ��� ���� */
	// Ÿ�̸� �߰�
	HRESULT AddTimer(const _tchar* _szTimerKey);
	// Ÿ�̸ӿ��� �ð� ��ȭ�� ����
	_double ComputeDeltaTime(const _tchar* _szTimerKey);

public:	/* ���������� ���� */
	// ���������� ��ü�� ����� ��ȯ �Լ��� ������ �Լ�
	HRESULT GetTransform(D3DTRANSFORMSTATETYPE _eTransform, _matrix* _pMatrix);
	// ī�޶��� ��������� �������� �Լ�
	HRESULT GetCameraWorldMatrix(_matrix* _vCameraDirection);

public:	/* �� ���� */	
	// ���� �޾ƿ� ���� ���� �����ϴ� �Լ�
	HRESULT SetCurrentScene(CScene* _pScene);
	// Update Scene
	HRESULT UpdateCurrentScene(_double _dDeltaTime);
	// Render Scene
	HRESULT RenderCurrentScene();

public: /* ���� ���� */
	// ���� �غ�
	HRESULT InitSound();
	// ������Ʈ
	HRESULT UpdateSound();
	// ä�ο� ���� ���
	HRESULT PlaySound(const _tchar* _szSoundKey, const _uint uiChannelID);
	// Ư�� ä���� ���� ����
	HRESULT StopSound(const _uint uiChannelID);
	// ��� ���� ����
	HRESULT StopAll();
	// ���� ����
	HRESULT SetVolume(_uint uiChannelID, _float _fVol);

private:
	// �׷��� ��ġ
	CGraphicDevice* m_pGraphicDevice = nullptr;
	// �Է� ��ġ
	CInputDevice* m_pInputDevice = nullptr;
	// ����Ÿ�� �Ŵ���
	CTargetManager* m_pTargetManager = nullptr;
	// ������Ʈ �Ŵ���
	CObjectManager* m_pOjectManager = nullptr;
	// ������Ʈ �Ŵ���
	CComponentManager* m_pComponentManager = nullptr;
	// �ݸ��� �Ŵ���
	CCollisionManager* m_pCollisionManager = nullptr;
	// Ÿ�̸� �Ŵ���
	CTimerManager* m_pTimerManager = nullptr;
	// ����Ʈ �Ŵ���
	CLightManager* m_pLightManager = nullptr;
	// ���� �Ŵ���
	CSoundManager* m_pSoundManager = nullptr;
	// ����������(�� ��ȯ ��İ� ���� ����� ����)
	CPipeline* m_pPipeline = nullptr;
	// ���� ��
	CScene* m_pCurrentScene	= nullptr;

public:
	// Free
	virtual void Free() override;

	// static �Լ�
public:
	//���� ���� �۾��� �����ϴ� �Լ�(static)
	static HRESULT ReleaseEngine();
};

END

#endif // !__MANAGEMENT_H__
