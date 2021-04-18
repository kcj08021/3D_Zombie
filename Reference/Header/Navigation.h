#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "Component.h"

BEGIN(Engine)
class CCell;
class CShader;
class ENGINE_DLL CNavigation:
	public CComponent{
private:
	// 생성자
	explicit CNavigation(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CNavigation(const CNavigation& _Navigation);
	// 소멸자
	virtual ~CNavigation() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(const _tchar* _szFilePath);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);

public:
	_bool MovingCheck(_vec3& _vMoveResult);
	// 네비메쉬의 렌더
	HRESULT RenderNaviMash();

private:
	// 네비게이션 메쉬 각 삼각형들을 보관하는 컨테이너
	vector<CCell*> m_vecCell;
	typedef vector<CCell*> CELL_VECTOR;

	CShader* m_pShaderCom = nullptr;
	// 삼각형의 총 갯수
	_uint m_uiCellCount = 0;
	// 현재 부모 객체가 있는 네비 메쉬의 인덱스
	_uint m_uiCurrentIndex = 0;

private:
	// 각 삼각형들의 이웃을 세팅하는 함수
	HRESULT SetttingNeigbor();

public:
	// 원본 생성 함수(static)
	static CNavigation* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);
	
public:
	// Free
	virtual void Free();
};

END

#endif // !__NAVIGATION_H__
