#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include "Base.h"
#include "..\Header\Component.h"

// 엔진에서 구현된 컴포넌트들
#include "..\Header\Renderer.h"
#include "..\Header\Shader.h"
#include "..\Header\Texture.h"
#include "..\Header\VIBufferRect.h"
#include "..\Header\VIBufferTerrain.h"
#include "..\Header\VIBufferCube.h"
#include "..\Header\Transform.h"
#include "..\Header\Picker.h"
#include "..\Header\Pickee.h"
#include "..\Header\StaticMesh.h"
#include "..\Header\DynamicMesh.h"
#include "..\Header\Collider.h"
#include "..\Header\Navigation.h"
#include "..\Header\Frustum.h"


BEGIN(Engine)

class CComponentManager final: public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CComponentManager)

private:
	// 생성자
	explicit CComponentManager();
	// 소멸자
	virtual ~CComponentManager() = default;

public:
	// 씬 갯수에 맞춰서 레어이맵 배열 동적 할당
	HRESULT ReserveComponentMapArray(_uint _uiSceneCount);

	//컴포넌트 추가 관련
public:
	// 프로토타입 컴포넌트 추가
	HRESULT AddProtoComponent(const _tchar* _szKey, _uint _uiIndex, CComponent* _pComponent);
	// 프로토타입을 복제하여 반환
	CComponent* AddCloneComponent(const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument);

	// 등록되어 있는 프로토타입 컴포넌트를 제거
	HRESULT ReleaseProtoComponent(const _tchar* _szKey, _uint _uiIndex);

private:
	// 컴포넌트의 프로토타입을 보관하는 컨테이너(맵)의 배열(씬 별로 보관)
	unordered_map<const _tchar*, CComponent*>* m_pComponentMapArray = nullptr;
	typedef unordered_map<const _tchar*, CComponent*> COMPONENT_MAP;
	// 씬 갯수 변수
	_uint m_uiSceneCount = 0;

private:
	// 컨테이너 검색 함수
	CComponent* FindPrototype(const _tchar* _szFindString, _uint _uiIndex);

	// CBase을(를) 통해 상속됨
public:
	// Free
	virtual void Free();
};

END

#endif // !__COMPONENT_MANAGER_H__


