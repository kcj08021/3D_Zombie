#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBufferCube;
END

BEGIN(Client)
class CSkyBox final:
	public CGameObject{
public:
	typedef struct tagSkyBoxDesc{
		const _tchar* szTextureComTag;
}SKYBOX_DESC;

private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	explicit CSkyBox(const CSkyBox& _SkyBox);
	virtual ~CSkyBox() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) override;

public:
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendereCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBufferCube* m_pVIBufferCom = nullptr;

public:
	static CSkyBox* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDeivce);
	virtual CGameObject* CreateClone(void* _pArgument) override;

public:
	virtual void Free();
};

END

#endif // !__SKYBOX_H__

