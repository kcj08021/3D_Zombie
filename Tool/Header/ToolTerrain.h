#ifndef __TOOL_TERRAIN_H__
#define __TOOL_TERRAIN_H__

#include "GameObject.h"

// 전방 선언
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
	// 지형 버퍼를 받아와 설정함
	HRESULT SettingVIBufferTerrain();
	// 지정된 높이와 범위로 지형 편집
	HRESULT EditHeight(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);
	// 높이맵을 저장
	HRESULT SaveHeightMap(const _tchar* _szPath);
	// 높이맵을 불러오기
	HRESULT LoadHeightMap(const _tchar* _szPath);


	// Set
public:
	// 텍스쳐를 받아와 설정함
	HRESULT SetTerrainTexture(CTexture* _pTextureCom);
	void SetShaderPass(_uint _uiShaderPass){
		m_uiShaderPass = _uiShaderPass;
	}
	// 높이 편집시 변경되는 높이값으 설정함
	void SetHeightValue(_uint _uiHeightValue){
		m_uiHeightValue = _uiHeightValue;
	}

	// 브러쉬 크기를 받아와 설정함
	void SetBrushInfo(_uint _uiOuterSize, _uint _uiInnerSize){
		m_uiOuterCircleSize = _uiOuterSize;
		m_uiInnerCircleSize = _uiInnerSize;
	}
	void SetBrushCursor(_bool _bIsCursorRect){
		m_bIsCursorRect = _bIsCursorRect;
	}
	// 브러쉬의 모양을 받아와 설정함
	void SetBrushTexture(CTexture* _pTextureCom){
		m_pBrushTextureCom = _pTextureCom;
	}

private:
	// 셰이더 컴포넌트
	CShader* m_pShaderCom = nullptr;
	// 지형 텍스쳐 컴포넌트
	CTexture* m_pTextureCom = nullptr;
	// 필터 텍스쳐 컴포넌트
	CTexture* m_pFilterTextureCom = nullptr;
	// 브러쉬 텍스쳐 컴포넌트
	CTexture* m_pBrushTextureCom = nullptr;

	// 렌더러 컴포넌트
	CRenderer* m_pRendererCom = nullptr;
	// 트랜스폼 컴포넌트
	CTransform* m_pTransformCom = nullptr;
	// 지형 버텍스 버퍼 컴포넌트
	CVIBufferTerrain* m_pVIBufferTerrainCom = nullptr;
	// 픽키 컴포넌트
	CPickee* m_pPickeeCom = nullptr;

	// 셰이더 패스 인덱스 변수
	_uint m_uiShaderPass = 0;

	// 클릭시 변경되는 높이
	_uint m_uiHeightValue = 0;

	// 브러쉬 크기 변수
	_uint m_uiOuterCircleSize = 0;
	_uint m_uiInnerCircleSize = 0;

	// 브러쉬 커서 모양, true일때 사각형
	_bool m_bIsCursorRect = false;

private:
	// 컴포넌트 클론을 받아옴
	HRESULT SettingCloneComponent();
	// 원형의 커서로 높이를 변경함
	HRESULT EditHeightToCircle(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);
	// 사각형의 커서로 높이를 변경함
	HRESULT EditHeightToRect(const _vec3& _vPos, _double _dDeltaTime, _bool _bIsContinuallyMode);

public:
	// 원본 생성 함수(static)
	static CToolTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CGameObject* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();

};

END

#endif // !__TOOL_TERRAIN_H__
