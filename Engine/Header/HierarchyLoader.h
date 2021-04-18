#ifndef __HIERARCHY_LOADER_H__
#define __HIERARCHY_LOADER_H__

#include "..\Header\Base.h"

BEGIN(Engine)

class CHierarchyLoader:
	public ID3DXAllocateHierarchy, public CBase{
private:
	// ������
	explicit CHierarchyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// �Ҹ���
	virtual ~CHierarchyLoader() = default;

private:
	// Initialize
	HRESULT InitHierarchyLoader(const _tchar* _szFilePath);

	// ID3DXAllocateHierarchy��(��) ���� ��ӵ�
public:
	// ������ ����
	STDMETHOD(CreateFrame)(THIS_ LPCSTR _Name, LPD3DXFRAME * _ppNewFrame);
	// �޽� �����̳� ����
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR _Name, CONST D3DXMESHDATA * _pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer);
	// �ҷ��� �������� �Ҵ� ����
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME _pFrameToFree);
	// �޽� �����̳� �Ҵ� ����
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER _pMeshContainerToFree);

private:
	// �׷��� ��ġ ����
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// �ؽ��ĸ� �ε��ϱ� ���� ���� ���
	const _tchar* m_szMeshPath = nullptr;

private:
	// ���� �޽��� �̸��� �Ҵ��ϱ� ���� �Լ�
	HRESULT AllocCopyName(char** _ppSrcName, const char* _pDestName);
	// �ؽ��� �ε�� �̸� ������ �ϱ� ���� �Լ�
	HRESULT ChangeTextureFileName(_tchar* _szOut, const _tchar* _szIn, const _tchar* _szSrcText, const _tchar* _szDestText);

public:
	// ���� �Լ�(static)
	static CHierarchyLoader* CreateHierachyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szMeshPath);
	// Free
	virtual void Free();	
};

END

#endif // !__HIERARCHY_LOADER_H__

