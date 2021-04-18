#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Base.h"
BEGIN(Engine)

class CShader;
class CVIBufferViewportRect;
class CLight;

class ENGINE_DLL CLight final:
	public CBase{
private:
	// 생성자
	explicit CLight(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
	virtual ~CLight() = default;

public:
	// 조명 정보 가져오는 함수
	const D3DLIGHT9* GetLight() const{
		return &m_tLightDesc;
	}
	D3DLIGHT9* GetLight(){
		return &m_tLightDesc;
	}
	_bool IsTurnOn(){
		return m_bIsTurnOn;
	}
	_float GetViewZ() const{
		return m_fViewZ;
	}
public:
	void SetTurnLight(_bool _bIsTurnOn){
		m_bIsTurnOn = _bIsTurnOn;
	}
	void SetLightPos(_vec3& _vPos){
		m_tLightDesc.Position = _vPos;
	}

private:
	// Initialize
	HRESULT InitLight(const D3DLIGHT9* _pLightDesc);

public:
	HRESULT RenderLight(CShader* _pShader);

private:
	// 그래픽 장치 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 조명 정보 구조체
	D3DLIGHT9 m_tLightDesc;
	// 조명을 그리는 버퍼
	CVIBufferViewportRect* m_pRenderBuffer;
	// 조명이 켜져있는 상태인지 확인하는 변수
	_bool m_bIsTurnOn = false;
	// 뷰 스페이스상의 Z 위치
	_float m_fViewZ = 0.f;

private:
	HRESULT ComputeViewZ();

public:
	// 생성 함수(static)
	static CLight* Create(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc);
	// Free
	virtual void Free();
};
END

#endif // !__LIGHT_H__


