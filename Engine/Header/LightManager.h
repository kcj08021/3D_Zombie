#ifndef __LIGHT_MANAGER_H__
#define __LIGHT_MANAGER_H__

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class CShader;

class CLightManager final:
	public CBase{
	// �̱��� ����
	DECLARE_SINGLETON(CLightManager)
private:
	// ������
	explicit CLightManager();
	// �Ҹ���
	virtual ~CLightManager() = default;

public:
	const D3DLIGHT9* GetLight(_uint _uiIndex) const;

public:
	HRESULT RenderLight(CShader* _pShader);

public:
	// ����Ʈ �߰�
	HRESULT AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLight, CLight** _ppOutLight);
	// ����Ʈ ����
	HRESULT ReleaseLight(CLight* _pLight);
	// ����Ʈ �ʱ�ȭ
	HRESULT ResetLight();

private:
	// ����Ʈ�� ������ �����̳� ����
	list<CLight*> m_LightList;
	typedef list<CLight*> LIGHT_LIST;

public:
	// Free
	virtual void Free();
};

END


#endif // !__LIGHT_MANAGER_H__
