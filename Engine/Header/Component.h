#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CComponent abstract: public CBase{
protected:
	// 생성자
	explicit CComponent(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CComponent(const CComponent& _Component);
	// 소멸자
	virtual ~CComponent() = default;

	// Get
public:
	LPDIRECT3DDEVICE9 GetGraphicDevice(){
		return m_pGraphicDevice;
	}

public:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) PURE;

protected:
	_bool IsClone(){
		return m_bIsClone;
	}

private:
	// 그래픽 장치 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 복제된 컴포넌트인지 확인하는 bool 변수
	_bool m_bIsClone = false;

public:
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument) PURE;
	// Free
	virtual void Free();
};

END

#endif // !__COMPONENT_H__

