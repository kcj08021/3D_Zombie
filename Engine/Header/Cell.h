#ifndef __CELL_H__
#define __CELL_H__

#include "Base.h"

BEGIN(Engine)

class CCell final:
	public CBase{
public:
	enum NEIGHBOR_TYPE{
		NEIGHBOR_1_2, NEIGHBOR_2_3, NEIGHBOR_3_1, NEIGHBOR_END
	};
	enum LINE_TYPE{
		LINE_1_2, LINE_2_3, LINE_3_1, LINE_END
	};
private:
	explicit CCell(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual ~CCell() = default;

private:
	HRESULT InitCell(const POLYGON_POSITION& _refPolygon, _uint _uiIndex);

public:
	const POLYGON_POSITION& GetPolygonPos() const{
		return m_tPolygon;
	}
	_uint GetIndex() const{
		return m_uiIndex;
	}

	void SettingNeighbor(NEIGHBOR_TYPE eType, CCell* _Cell){
		m_NeighborCell[eType] = _Cell;
	}

public:
	_bool CheckInside(_vec3& _vMovingResult, const CCell** _pNeighbor) const;

public:
	//
	_bool ComparePoint(const _vec3 _vSrc, const _vec3 _vDest) const;
	HRESULT RenderCell() const;

private:
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	POLYGON_POSITION m_tPolygon;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = nullptr;
	CCell* m_NeighborCell[NEIGHBOR_END];
	_uint m_uiIndex = 0;
	_vec3 m_vDirection[LINE_END];
	_vec3 m_vNormal[LINE_END];

public:
	static CCell* CreateCell(LPDIRECT3DDEVICE9 _pGraphicDevice, const POLYGON_POSITION& _refPolygon, _uint _uiIndex);

public:
	virtual void Free();
};

END

#endif // !__CELL_H__
