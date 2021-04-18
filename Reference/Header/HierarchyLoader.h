#ifndef __HIERARCHY_LOADER_H__
#define __HIERARCHY_LOADER_H__

#include "..\Header\Base.h"

BEGIN(Engine)

class CHierarchyLoader:
	public ID3DXAllocateHierarchy, public CBase{
private:
	// 생성자
	explicit CHierarchyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 소멸자
	virtual ~CHierarchyLoader() = default;

private:
	// Initialize
	HRESULT InitHierarchyLoader(const _tchar* _szFilePath);

	// ID3DXAllocateHierarchy을(를) 통해 상속됨
public:
	// 프레임 생성
	STDMETHOD(CreateFrame)(THIS_ LPCSTR _Name, LPD3DXFRAME * _ppNewFrame);
	// 메쉬 컨테이너 생성
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR _Name, CONST D3DXMESHDATA * _pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer);
	// 불러온 프레임의 할당 해제
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME _pFrameToFree);
	// 메쉬 컨테이너 할당 해제
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER _pMeshContainerToFree);

private:
	// 그래픽 장치 변수
	LPDIRECT3DDEVICE9 m_pGraphicDevice = nullptr;
	// 텍스쳐를 로드하기 위한 폴더 경로
	const _tchar* m_szMeshPath = nullptr;

private:
	// 뼈와 메쉬의 이름을 할당하기 위한 함수
	HRESULT AllocCopyName(char** _ppSrcName, const char* _pDestName);
	// 텍스쳐 로드시 이름 변경을 하기 위한 함수
	HRESULT ChangeTextureFileName(_tchar* _szOut, const _tchar* _szIn, const _tchar* _szSrcText, const _tchar* _szDestText);

public:
	// 생성 함수(static)
	static CHierarchyLoader* CreateHierachyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szMeshPath);
	// Free
	virtual void Free();	
};

END

#endif // !__HIERARCHY_LOADER_H__

