#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Component.h"

BEGIN(Engine)

class CGameObject;
class CTargetManager;
class CLightManager;
class CPipeline;
class CShader;
class CVIBufferViewportRect;

class ENGINE_DLL CRenderer final: public CComponent{
public:
	// ���� �׷� ����ü
	enum RENDER_GROUP{ RENDER_PRE, RENDER_NONALPHA, RENDER_PRIORITY, RENDER_ALPHA, RENDER_PRE_UI, RENDER_POST_UI, RENDER_END };
private:
	// ������
	explicit CRenderer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	/*explicit CRenderer(const CRenderer& _pComponent);	*/	// Renderer�� ���� ������ ����
	// �Ҹ���
	virtual ~CRenderer() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

public:
	_bool GetRenderDebugBuffer(){
		return m_bIsRenderDebugBuffer;
	}

public:
	void SetRenderDebugBuffer(_bool _bIsRenderDebugBuffer){
		m_bIsRenderDebugBuffer = _bIsRenderDebugBuffer;
	}

public:
	// ������ ������Ʈ �߰�
	HRESULT AddRenderList(RENDER_GROUP _eGroup, CGameObject* _pObj);
	// ���� ����
	HRESULT DoRender(_double _dDeltaTime);
private:
	// ������ ����� ������ �����̳�
	list<CGameObject*> m_RenderList[RENDER_END];
	// �����̳� Ÿ�� ������
	typedef list<CGameObject*> RENDER_LIST;
	// ����Ÿ�� �Ŵ��� ������ ����
	CTargetManager* m_pTargetManager = nullptr;
	// ���� �Ŵ��� ������ ����
	CLightManager* m_pLightManager = nullptr;
	// ���������� ������ ����
	CPipeline* m_pPipeline = nullptr;
	// ����Ÿ�ٵ��� ���� �ϱ����� ���̴�
	CShader* m_pBlendShader = nullptr;
	// ������ �������� �׸��� ���� ���̴�
	CShader* m_pLightAccShader = nullptr;
	// ������ ����Ÿ�ٵ��� �׷����� ����
	CVIBufferViewportRect* m_pRenderBuffer;
	// ����� ���۸� �������� üũ
	_bool m_bIsRenderDebugBuffer = false;

private:
	HRESULT RenderPreGroup(_double _dDeltaTime);
	HRESULT RenderNonAlphaGroup(_double _dDeltaTime);
	HRESULT RenderPriorityGroup(_double _dDeltaTime);
	HRESULT RenderLight();
	HRESULT RenderAlphaGroup(_double _dDeltaTime);
	HRESULT RenderUIGroup(_double _dDeltaTime);
	
private:
	HRESULT BlendRenderTarget();

public:
	// ������Ÿ�� ���� �Լ�
	static CRenderer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);

	// CComponent��(��) ���� ��ӵ�
public:
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);
	// Free
	virtual void Free();
};

END

#endif // !__RENDERER_H__
