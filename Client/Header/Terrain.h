#ifndef __TERRAIN_H__
#define __TERRAIN_H__

// Engine ���
#include "GameObject.h"

// ���� ����
BEGIN(Engine)
class CShader;
class CRenderer;
class CStaticMesh;
END

BEGIN(Client)

class CTerrain final:
	public CGameObject{
private:
	// ������
	explicit CTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CTerrain(const CTerrain& _Terrain);
	// �Ҹ���
	virtual ~CTerrain() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument);
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
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// ������ ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;
	// ����ƽ �޽� ������Ʈ
	CStaticMesh* m_pStaticMesh = nullptr;
	CPickee* m_pPickeeCom = nullptr;

public:
	// ���� ���� �Լ�(static)
	static CTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CGameObject * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__TERRAIN_H__
