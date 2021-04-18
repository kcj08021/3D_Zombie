#ifndef __LIGHT_MANAGER_H__
#define __LIGHT_MANAGER_H__

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class CShader;

class CLightManager final:
	public CBase{
	// 싱글톤 정의
	DECLARE_SINGLETON(CLightManager)
private:
	// 생성자
	explicit CLightManager();
	// 소멸자
	virtual ~CLightManager() = default;

public:
	const D3DLIGHT9* GetLight(_uint _uiIndex) const;

public:
	HRESULT RenderLight(CShader* _pShader);

public:
	// 라이트 추가
	HRESULT AddLight(LPDIRECT3DDEVICE9 _pGraphicDevice, const D3DLIGHT9* _pLight, CLight** _ppOutLight);
	// 라이트 삭제
	HRESULT ReleaseLight(CLight* _pLight);
	// 라이트 초기화
	HRESULT ResetLight();

private:
	// 라이트를 보관할 컨테이너 변수
	list<CLight*> m_LightList;
	typedef list<CLight*> LIGHT_LIST;

public:
	// Free
	virtual void Free();
};

END


#endif // !__LIGHT_MANAGER_H__
