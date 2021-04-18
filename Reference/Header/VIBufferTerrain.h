#ifndef __VI_BUFFER_TERRAIN_H__
#define __VI_BUFFER_TERRAIN_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBufferTerrain final:
	public CVIBuffer{
private:
	// 생성자
	explicit CVIBufferTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CVIBufferTerrain(const CVIBufferTerrain& _VIBufferTerrain);
	// 소멸자
	virtual ~CVIBufferTerrain() = default;
	
private:
	// Initialize with Prototype(지형 가로세로 크기)
	virtual HRESULT InitComponent_Proto(_uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fInterval = 1.f);
	// Initialize with Prototype(지형정보 BMP파일 경로)
	virtual HRESULT InitComponent_Proto(const _tchar* _szFilePath, _float _fInterval = 1.f);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;

public:
	// Render
	HRESULT RenderVIBufferTerrain();

public:
	void GetTerrainInfo(_uint& _uiVerticesCountX, _uint& _uiVerticesCountZ, _float& _fInterval){
		_uiVerticesCountX = m_uiVerticesCountX;
		_uiVerticesCountZ = m_uiVerticesCountZ;;
		_fInterval = m_fInterval;
	}
	HRESULT GetHeightMap(LPDIRECT3DTEXTURE9* pTexture, _float _fHeightMax);

	// Set
public:
	// 버텍스의 위치정보를 갱신하는 함수(버텍스 수는 그대로)
	HRESULT SetVerticesPosition(POLYGON_POSITION* _pPolygonPosition, _uint _uiPrimitiveCount);
	HRESULT SetVerticesHeightPosition(const _vec3& _vPos, _uint _uiInner, _uint _uiOuter, _float _fHeight, _float _fHeightMax, _double _dDeltaTime, _bool _bIsContinuallyMode);

private:
	// 지형의 X축 정점 갯수
	_uint m_uiVerticesCountX = 0;
	// 지형의 Z축 정점 갯수
	_uint m_uiVerticesCountZ = 0;
	// 지형의 정점간 거리
	_float m_fInterval = 0.f;


public:
	// 원본 생성 함수(static), 지형의 가로세로, 정점간격
	static CVIBufferTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fIntaval);
	// 원본 생성 함수(static), 지형정보 BMP파일 경로, 정점간격
	static CVIBufferTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath, _float _fInterval = 1.f);

	// 복제 생성 함수
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__VI_BUFFER_TERRAIN_H__
