#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

// Engine 헤더
#include "GameObject.h"

// 전방선언
BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBufferRect;
class CTexture;
END

BEGIN(Client)

class CBackground final: 
	public CGameObject{ 
private:
	// 생성자
	explicit CBackground(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CBackground(const CBackground& _pObj);
	// 소멸자
	virtual ~CBackground() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) override;
public:
	// LateInitailize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

private:
	// 렌더러 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 사각형 버텍스 컴포넌트 
	CVIBufferRect* m_pVertexBufferCom = nullptr;
	// 텍스쳐 컴포넌트
	CTexture* m_pTextureCom = nullptr;

public:
	// Prototype 생성 함수(static)
	static CBackground* CreatePrototype(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// Clone 생성 함수
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:		
	// Free
	virtual void Free();

};

END

#endif // !__BACKGROUND_H__
