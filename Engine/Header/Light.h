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
	// ������
	explicit CLight(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// �Ҹ���
	virtual ~CLight() = default;

public:
	// ���� ���� �������� �Լ�
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
	// �׷��� ��ġ ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// ���� ���� ����ü
	D3DLIGHT9 m_tLightDesc;
	// ������ �׸��� ����
	CVIBufferViewportRect* m_pRenderBuffer;
	// ������ �����ִ� �������� Ȯ���ϴ� ����
	_bool m_bIsTurnOn = false;
	// �� �����̽����� Z ��ġ
	_float m_fViewZ = 0.f;

private:
	HRESULT ComputeViewZ();

public:
	// ���� �Լ�(static)
	static CLight* Create(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLightDesc);
	// Free
	virtual void Free();
};
END

#endif // !__LIGHT_H__


