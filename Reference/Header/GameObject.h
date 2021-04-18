#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Base.h"
#include "..\Header\ComponentManager.h"

BEGIN(Engine)

//전방 선언
class CComponent;

class ENGINE_DLL CGameObject abstract: 
	public CBase{
protected:
	// 생성자
	explicit CGameObject(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CGameObject(const CGameObject& _pObj);
	// 소멸자
	virtual ~CGameObject() = default;

	// Get
public:
	// 객체에서 사용중인 컴포넌트를 검색, 반환
	CComponent* GetComponent(const _tchar* _szKey){
		return FindComponent(_szKey);
	}
	// 그래픽 장치를 얻어오는 함수
	LPDIRECT3DDEVICE9 GetGraphicDevice() const{
		return m_pGraphicDevice;
	}
	// 뷰 스페이스상의 Z값을 얻어오는 함수
	_float GetViewZ(){
		return m_fViewZ;
	}
	_bool GetIsDeadObject(){
		return m_bIsDeadObject;
	}
public:
	// Initialize with Prototype
	virtual HRESULT InitGameObject_Proto();
	// Initialize with Clone
	virtual HRESULT InitGameObject_Clone(void* _pArgument) PURE;
	// LateInitialize
	virtual HRESULT LateInitGameObject();
	// Update
	virtual HRESULT UpdateGameObject(_double _dDeltaTime);
	// LateUpdate
	virtual HRESULT LateUpdateGameObject(_double _dDeltaTime);
	// Render
	virtual HRESULT RenderGameObject(_double _dDeltaTime);

protected: 
	// 컴포넌트를 복제 및 컨테이너에 보관하기 위한 함수
	HRESULT AddComponent(CComponent** _ppOutComponent, const _tchar* _szCloneKey, const _tchar* _szProtoKey, _uint _uiIndex, void* _pArgument = nullptr );
	// 가진 특정 컴포넌트를 지우는 함수
	HRESULT ReleaseComponent(CComponent** _ppComponent, const _tchar* _szCloneKey);
	// 뷰 스페이스상의 Z값을 계산함
	HRESULT ComputeViewZ(_vec3 _vWorldPos);

protected:
	// LateInitialize 확인 변수
	_bool m_bIsLateInit = false;
	_bool m_bIsDeadObject = false;

private:
	// 컴포넌트 컨테이너에서 검색하는 함수
	CComponent* FindComponent(const _tchar* _szFindString);

private:
	// 그래픽 장치 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 컴포넌트를 보관할 컨테이너(외부에서 검색하기 위한)
	unordered_map<const _tchar*, CComponent*> m_ComponentMap;
	typedef unordered_map<const _tchar*, CComponent*> COMPONENT_MAP;
	// 뷰 스페이스 상에서의 Z값(알파 블렌딩 객체에 한에서 사용함)
	_float m_fViewZ = 0.f;
	
public:
	// Clone(프로토타입을 통한 객체 생성을 위한 함수)
	virtual CGameObject* CreateClone(void* _pArgument) PURE;

	// CBase을(를) 통해 상속됨
public:
	// Free
	virtual void Free() override;
};

END
#endif // !__GAME_OBJECT_H__

