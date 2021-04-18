#ifndef __MAINAPP_H__
#define __MAINAPP_H__
// Client ���
#include "Client_Headers.h"

// Engine ���
#include "Base.h"
#include "Management.h"

BEGIN(Client)
class CMainApp final: public CBase{
private:
	// ������
	explicit CMainApp();
	// �Ҹ���
	virtual ~CMainApp() = default;

private:
	// Initialize
	HRESULT InitMainApp();
public:
	// Update
	HRESULT UpdateMainApp(_double _dDeltaTime);
	// Render
	HRESULT RenderMainApp(_double _dDeltaTime);

protected:

private:
	// �Ŵ�����Ʈ ����, ������ Ŭ���̾�Ʈ ������ ����
	CManagement* m_pManagement = nullptr;
	// �׷��� ����̽� ����, �� �״�� ����̽� ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// ������ ������Ʈ ����
	CRenderer* m_pRendererCom = nullptr;

	// �Է� ���� ������Ʈ ���� Ȯ�� ����(���н� �ѹ��� �޼��� �ڽ� ��������)
	_bool m_bIsInputStateUpdateFailure = false;

private:
	// �ʱ⿡ ��� �������� �����ϴ� �Լ�
	HRESULT SettingDefault(CGraphicDevice::MODE _eMode, _uint _uiWinCX, _uint _uiWinCY);
	// ���� �����ϴ� �Լ�
	HRESULT SettingScene(SCENE_TYPE _eSceneType);

	/* �� �κп� �ִ� �Լ��� �׽�Ʈ�� �Լ��� */
	// ������ ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddRenderComponent();
	// ���̴� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddShaderComponent();
	// �簢���� �׸��� ���� ���ؽ� ���۸� ����� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddVIBufferRectComponent();
	// ������ �׸��� ���� ���ؽ� ���۸� ����� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddVIBufferTerrainComponent();
	// �ؽ��ĸ� �ε��Ͽ� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddTextureComponent();
	// CTransform �� ����Ʈ�� ����ϴ� �Լ�
	HRESULT AddTransformComponent();
	// ��ŷ�� ���� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddPickingComponent();
	// �ݶ��̴� ������Ʈ�� ����ϴ� �Լ�
	HRESULT AddColliderComponent();
	// ��ī�̹ڽ� ������Ʈ�� ������Ÿ���� ����ϴ� �Լ�
	HRESULT AddSkyBoxObject();
	// ����ü ������Ʈ�� ������Ÿ���� ����ϴ� �Լ�
	HRESULT AddFrustumComponent();
public:
	// ���� �Լ�(static)
	static CMainApp* Create();

public:
	// Free
	virtual void Free() override;
};
END
#endif // !__MAINAPP_H__