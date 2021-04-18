#ifndef __VI_BUFFER_TERRAIN_H__
#define __VI_BUFFER_TERRAIN_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBufferTerrain final:
	public CVIBuffer{
private:
	// ������
	explicit CVIBufferTerrain(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CVIBufferTerrain(const CVIBufferTerrain& _VIBufferTerrain);
	// �Ҹ���
	virtual ~CVIBufferTerrain() = default;
	
private:
	// Initialize with Prototype(���� ���μ��� ũ��)
	virtual HRESULT InitComponent_Proto(_uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fInterval = 1.f);
	// Initialize with Prototype(�������� BMP���� ���)
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
	// ���ؽ��� ��ġ������ �����ϴ� �Լ�(���ؽ� ���� �״��)
	HRESULT SetVerticesPosition(POLYGON_POSITION* _pPolygonPosition, _uint _uiPrimitiveCount);
	HRESULT SetVerticesHeightPosition(const _vec3& _vPos, _uint _uiInner, _uint _uiOuter, _float _fHeight, _float _fHeightMax, _double _dDeltaTime, _bool _bIsContinuallyMode);

private:
	// ������ X�� ���� ����
	_uint m_uiVerticesCountX = 0;
	// ������ Z�� ���� ����
	_uint m_uiVerticesCountZ = 0;
	// ������ ������ �Ÿ�
	_float m_fInterval = 0.f;


public:
	// ���� ���� �Լ�(static), ������ ���μ���, ��������
	static CVIBufferTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, _uint _uiVerticesCountX, _uint _uiVerticesCountZ, _float _fIntaval);
	// ���� ���� �Լ�(static), �������� BMP���� ���, ��������
	static CVIBufferTerrain* CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szFilePath, _float _fInterval = 1.f);

	// ���� ���� �Լ�
	virtual CComponent * CreateClone(void* _pArgument) override;

public:
	// Free
	virtual void Free();
};

END

#endif // !__VI_BUFFER_TERRAIN_H__
