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
	// 렌더 그룹 열거체
	enum RENDER_GROUP{ RENDER_PRE, RENDER_NONALPHA, RENDER_PRIORITY, RENDER_ALPHA, RENDER_PRE_UI, RENDER_POST_UI, RENDER_END };
private:
	// 생성자
	explicit CRenderer(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	/*explicit CRenderer(const CRenderer& _pComponent);	*/	// Renderer는 복사 생성자 없음
	// 소멸자
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
	// 렌더할 오브젝트 추가
	HRESULT AddRenderList(RENDER_GROUP _eGroup, CGameObject* _pObj);
	// 렌더 시작
	HRESULT DoRender(_double _dDeltaTime);
private:
	// 렌더할 대상을 보관할 컨테이너
	list<CGameObject*> m_RenderList[RENDER_END];
	// 컨테이너 타입 재정의
	typedef list<CGameObject*> RENDER_LIST;
	// 렌더타겟 매니저 포인터 변수
	CTargetManager* m_pTargetManager = nullptr;
	// 조명 매니저 포인터 변수
	CLightManager* m_pLightManager = nullptr;
	// 파이프라인 포인터 변수
	CPipeline* m_pPipeline = nullptr;
	// 렌더타겟들을 블렌드 하기위한 셰이더
	CShader* m_pBlendShader = nullptr;
	// 조명의 누적값을 그리기 위한 셰이더
	CShader* m_pLightAccShader = nullptr;
	// 블렌드한 렌더타겟들이 그려지는 버퍼
	CVIBufferViewportRect* m_pRenderBuffer;
	// 디버그 버퍼를 렌더할지 체크
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
	// 프로토타입 생성 함수
	static CRenderer* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);

	// CComponent을(를) 통해 상속됨
public:
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);
	// Free
	virtual void Free();
};

END

#endif // !__RENDERER_H__
