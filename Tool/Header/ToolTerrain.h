#ifndef __TOOL_TERRAIN_H__
#define __TOOL_TERRAIN_H__

#include "GameObject.h"

// ���� ����
BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBufferTerrain;
class CPickee;
END

BEGIN(Tool)

class CToolTerrain final
	: public CGameObject{
private:
	explicit CToolTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice);
	explicit CToolTerrain(const CToolTerrain& _ToolTerrain);
	virtual ~CToolTerrain() = default;

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

public:
	// ���� ���۸� �޾ƿ� ������
	HRESULT SettingVIBufferTerrain();
	// ������ ���̿� ������ ���� ����
	HRESULT EditHeight(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);
	// ���̸��� ����
	HRESULT SaveHeightMap(const _tchar* _szPath);
	// ���̸��� �ҷ�����
	HRESULT LoadHeightMap(const _tchar* _szPath);


	// Set
public:
	// �ؽ��ĸ� �޾ƿ� ������
	HRESULT SetTerrainTexture(CTexture* _pTextureCom);
	void SetShaderPass(_uint _uiShaderPass){
		m_uiShaderPass = _uiShaderPass;
	}
	// ���� ������ ����Ǵ� ���̰��� ������
	void SetHeightValue(_uint _uiHeightValue){
		m_uiHeightValue = _uiHeightValue;
	}

	// �귯�� ũ�⸦ �޾ƿ� ������
	void SetBrushInfo(_uint _uiOuterSize, _uint _uiInnerSize){
		m_uiOuterCircleSize = _uiOuterSize;
		m_uiInnerCircleSize = _uiInnerSize;
	}
	void SetBrushCursor(_bool _bIsCursorRect){
		m_bIsCursorRect = _bIsCursorRect;
	}
	// �귯���� ����� �޾ƿ� ������
	void SetBrushTexture(CTexture* _pTextureCom){
		m_pBrushTextureCom = _pTextureCom;
	}

private:
	// ���̴� ������Ʈ
	CShader* m_pShaderCom = nullptr;
	// ���� �ؽ��� ������Ʈ
	CTexture* m_pTextureCom = nullptr;
	// ���� �ؽ��� ������Ʈ
	CTexture* m_pFilterTextureCom = nullptr;
	// �귯�� �ؽ��� ������Ʈ
	CTexture* m_pBrushTextureCom = nullptr;

	// ������ ������Ʈ
	CRenderer* m_pRendererCom = nullptr;
	// Ʈ������ ������Ʈ
	CTransform* m_pTransformCom = nullptr;
	// ���� ���ؽ� ���� ������Ʈ
	CVIBufferTerrain* m_pVIBufferTerrainCom = nullptr;
	// ��Ű ������Ʈ
	CPickee* m_pPickeeCom = nullptr;

	// ���̴� �н� �ε��� ����
	_uint m_uiShaderPass = 0;

	// Ŭ���� ����Ǵ� ����
	_uint m_uiHeightValue = 0;

	// �귯�� ũ�� ����
	_uint m_uiOuterCircleSize = 0;
	_uint m_uiInnerCircleSize = 0;

	// �귯�� Ŀ�� ���, true�϶� �簢��
	_bool m_bIsCursorRect = false;

private:
	// ������Ʈ Ŭ���� �޾ƿ�
	HRESULT SettingCloneComponent();
	// ������ Ŀ���� ���̸� ������
	HRESULT EditHeightToCircle(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);
	// �簢���� Ŀ���� ���̸� ������
	HRESULT EditHeightToRect(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);

public:
	// ���� ���� �Լ�(static)
	static CToolTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ���� �Լ�
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();

};

END

#endif // !__TOOL_TERRAIN_H__
