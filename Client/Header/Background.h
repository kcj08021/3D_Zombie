#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

// Engine ���
#include "GameObject.h"

// ���漱��
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
	// ������
	explicit CBackground(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CBackground(const CBackground& _pObj);
	// �Ҹ���
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
	// ������ ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// �簢�� ���ؽ� ������Ʈ 
	CVIBufferRect* m_pVertexBufferCom = nullptr;
	// �ؽ��� ������Ʈ
	CTexture* m_pTextureCom = nullptr;

public:
	// Prototype ���� �Լ�(static)
	static CBackground* CreatePrototype(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// Clone ���� �Լ�
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:		
	// Free
	virtual void Free();

};

END

#endif // !__BACKGROUND_H__
