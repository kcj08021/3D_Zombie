#ifndef __PICKEE_H__
#define __PICKEE_H__

#include "Component.h"
BEGIN(Engine)
class CTransform;
class CGameObject;

class ENGINE_DLL CPickee final:
	public CComponent{
public:
	// 피킹 대상의 세부설정값(pArgument)
	enum PICKEE_TYPE{
		TYPE_TERRAIN, TYPE_OBJECT, TYPE_END
	};
	typedef struct tagPickeeDesc{
		// 피킹 대상 객체의 주소
		CGameObject* pGameObj = nullptr;
		union{
			// 폴리곤(정점 3개) 위치의 배열
			const POLYGON_POSITION* pPolygonPosition;
			// 메쉬의 포인터
			LPD3DXMESH pMesh;
		};
		// 정점으로 그려지는 폴리곤의 갯수(0이면 메쉬)
		_uint uiPrimitiveCount = 0;
		// 피킹 대상의 트랜스폼 컴포넌트의 포인터
		const _matrix* pWolrdMatrix = nullptr;
	}PICKEE_DESC;


private:
	// 생성자
	explicit CPickee(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CPickee(const CPickee& _Pickee);
	// 소멸자
	virtual ~CPickee() = default;


private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto();
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument);


public:
	HRESULT ChangePickeeDesc(PICKEE_DESC* _pPickeeDesc);
	HRESULT RemovePickeeDesc();

private:
	PICKEE_DESC m_tPickeeDesc;

public:
	// 원본 생성 함수(static)
	static CPickee* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};
END
#endif // !__PICKEE_H__
