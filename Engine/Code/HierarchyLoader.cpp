#include "..\Header\HierarchyLoader.h"

// 생성자
CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: m_pGraphicDevice(_pGraphicDevice)
	, m_szMeshPath(nullptr){
	SafeAddRef(m_pGraphicDevice);
}

// Initialize
HRESULT CHierarchyLoader::InitHierarchyLoader(const _tchar* _szFilePath){
	m_szMeshPath = _szFilePath;

	return NOERROR;
}

// 프레임 생성
STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR _Name, LPD3DXFRAME * _ppNewFrame){
	// D3DXFRAME을 상속받아 만든 구조체임
	D3DXFRAME_DERIVED* pFrameDrived = new D3DXFRAME_DERIVED;
	::ZeroMemory(pFrameDrived, sizeof(D3DXFRAME_DERIVED));

	// 프레임의 이름을 생성
	if(nullptr != _Name)
		AllocCopyName(&pFrameDrived->Name, _Name);

	// 각 프레임의 변환 행렬을 미리 항등행렬로 만들고, 따로 상속받아 만들어둔 멤버인 CombinedTransformationMatrix도 항등행렬로 만듬
	pFrameDrived->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pFrameDrived->TransformationMatrix);

	// 만들어진 프레임을 인자로 들어온 변수에 넣어줌(출력)
	*_ppNewFrame = pFrameDrived;

	return NOERROR;
}

// 메쉬 컨테이너 생성
STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR _Name, CONST D3DXMESHDATA * _pMeshData, CONST D3DXMATERIAL * _pMaterials, CONST D3DXEFFECTINSTANCE * _pEffectInstances, DWORD _NumMaterials, CONST DWORD * _pAdjacency, LPD3DXSKININFO _pSkinInfo, LPD3DXMESHCONTAINER * _ppNewMeshContainer){
	D3DXMESHCONTAINER_DERIVED* pMeshContainerDrived = new D3DXMESHCONTAINER_DERIVED;
	::ZeroMemory(pMeshContainerDrived, sizeof(D3DXMESHCONTAINER_DERIVED));

	// 메쉬 컨테이너의 이름을 할당하여 저장
	if(nullptr != _Name)
		AllocCopyName(&pMeshContainerDrived->Name, _Name);

	// 메쉬 타입(메쉬 타입이 메쉬)
	pMeshContainerDrived->MeshData.Type = D3DXMESHTYPE_MESH;

	// 메쉬 정보를 얻기위해 따로 변수로 받아옴
	LPD3DXMESH pMesh = _pMeshData->pMesh;
	SafeAddRef(pMesh);

	//// FVF에 맞춰서 메쉬를 복제함
	//if(FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() /*| D3DFVF_NORMAL*/, m_pGraphicDevice, &(pMeshContainerDrived->MeshData.pMesh))))
	//	return E_FAIL;

	// 메쉬의 정점 정보를 얻기위한 구조체
	D3DVERTEXELEMENT9 Decl[MAX_FVF_DECL_SIZE]; /*= {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};*/

	// 메쉬의 정점 정보 가져옴
	pMesh->GetDeclaration(Decl);

	// 가져온 메쉬 정보로 메쉬를 복제함
	if(FAILED(pMesh->CloneMesh(pMesh->GetOptions(), Decl, m_pGraphicDevice, &pMeshContainerDrived->MeshData.pMesh)))
		return E_FAIL;
	
	SafeRelease(pMesh);

	// 메쉬 컨테이너에 재질 갯수 저장
	pMeshContainerDrived->NumMaterials = _NumMaterials;
	
	// 메쉬 컨테이너에 재질 저장
	pMeshContainerDrived->pMaterials = new D3DXMATERIAL[_NumMaterials];
	::memcpy(pMeshContainerDrived->pMaterials, _pMaterials, sizeof(D3DXMATERIAL)* _NumMaterials);

	// 메쉬 컨테이너에 상속받아 넣은 멤버인 메쉬 텍스쳐의 공간을 할당
	pMeshContainerDrived->pMeshTexture = new MESH_TEXTURE[pMeshContainerDrived->NumMaterials];
	::ZeroMemory(pMeshContainerDrived->pMeshTexture, sizeof(MESH_TEXTURE) * pMeshContainerDrived->NumMaterials);

	_tchar szFilePath[MAX_PATH] = L"";

	// 메쉬 컨테이너에 경로에 들어있는 텍스쳐 로드하여 저장함
	for(_uint i = 0; i < pMeshContainerDrived->NumMaterials; ++i){
		// 텍스쳐 파일의 폴더 경로 설정
		::lstrcpy(szFilePath, m_szMeshPath);

		// 재질 정보에 들어있는 텍스쳐 파일의 이름을 가져옴(멀티바이트->와이드바이트로 변환)
		_tchar szFileName[MAX_PATH] = L"";

		::MultiByteToWideChar(CP_ACP, 0, pMeshContainerDrived->pMaterials[i].pTextureFilename, strlen(pMeshContainerDrived->pMaterials[i].pTextureFilename), szFileName, MAX_PATH);

		// 폴더 경로와 합쳐서 실제 파일의 위치로 만듬
		::lstrcat(szFilePath, szFileName);

		_tchar		szOut[MAX_PATH] = L"";

		// 디퓨즈 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"col.dds");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_DIFFUSE]);

		// 노말맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"nml.tga");

		LPDIRECT3DTEXTURE9 pHieghtTexture = nullptr;
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pHieghtTexture);
		if(nullptr != pHieghtTexture){
			D3DSURFACE_DESC desc;
			pHieghtTexture->GetLevelDesc(0, &desc);
			D3DXCreateTexture(m_pGraphicDevice, desc.Width, desc.Height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_NORMAL]);
			D3DXComputeNormalMap(pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_NORMAL], pHieghtTexture, nullptr, 0, D3DX_CHANNEL_RED, 5.f);
			SafeRelease(pHieghtTexture);
		}

		// 스펙큘러맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"spc.tga");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_SPECULAR]);

		// 이미시브맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"emi");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_EMISSIVE]);
	}

	// 메쉬 컨테이너에 어드전시 저장
	pMeshContainerDrived->pAdjacency = new _ulong[pMeshContainerDrived->MeshData.pMesh->GetNumFaces() * 3];
	memcpy(pMeshContainerDrived->pAdjacency, _pAdjacency, sizeof(_ulong) * pMeshContainerDrived->MeshData.pMesh->GetNumFaces() * 3);

	// 메쉬 컨테이너에 스킨인포 저장
	pMeshContainerDrived->pSkinInfo = _pSkinInfo;
	SafeAddRef(pMeshContainerDrived->pSkinInfo);

	// 메쉬 컨테이너에 프레임의 갯수 저장
	pMeshContainerDrived->dwNumFrames = pMeshContainerDrived->pSkinInfo->GetNumBones();

	// 메쉬 컨테이너에 오프셋 행렬을 배열로 만든뒤, 저장
	/* 뼈와 메쉬의 로컬이 일치하지 않기 떄문에 오프셋이 필요함 */
	pMeshContainerDrived->pOffsetMatrices = new _matrix[pMeshContainerDrived->dwNumFrames];
	for(_ulong i = 0; i < pMeshContainerDrived->dwNumFrames; ++i){
		pMeshContainerDrived->pOffsetMatrices[i] = *pMeshContainerDrived->pSkinInfo->GetBoneOffsetMatrix(i);
	}

	// 부모 뼈들을 보관할 행렬 포인터를 배열로 미리 할당해둠
	/* 메쉬 컨테이너를 만든 직후엔 값을 채울수가 없음 */
	pMeshContainerDrived->ppCombinedTransformationMatrices = new _matrix*[pMeshContainerDrived->dwNumFrames];
	
	// 렌더시에 이용되는 행렬을 보관할 배열을 미리 할당해둠
	/* 렌더시에 계산되는 행렬이기에 현재 값을 채울수가 없음 */
	pMeshContainerDrived->pRenderingMatrices = new _matrix[pMeshContainerDrived->dwNumFrames];
	
	// 가장 초기상태의 메쉬를 따로 메쉬 컨테이너에서 보관함
	/* 이 메쉬는 절대로 변하지 않게 함 */
	if(FAILED(pMeshContainerDrived->MeshData.pMesh->CloneMesh(pMeshContainerDrived->MeshData.pMesh->GetOptions(), Decl, m_pGraphicDevice, &pMeshContainerDrived->pOriginalMesh)))
		return E_FAIL;

	*_ppNewMeshContainer = pMeshContainerDrived;

	return NOERROR;
}

// 불러온 프레임의 할당 해제
STDMETHODIMP CHierarchyLoader::DestroyFrame(LPD3DXFRAME _pFrameToFree){
	SafeDelete_Array(_pFrameToFree->Name);

	if(nullptr != _pFrameToFree->pMeshContainer)
		DestroyMeshContainer(_pFrameToFree->pMeshContainer);

	if(nullptr != _pFrameToFree->pFrameFirstChild)
		DestroyFrame(_pFrameToFree->pFrameFirstChild);

	if(nullptr != _pFrameToFree->pFrameSibling)
		DestroyFrame(_pFrameToFree->pFrameSibling);

	SafeDelete(_pFrameToFree);

	return NOERROR;
}

// 메쉬 컨테이너 할당 해제
STDMETHODIMP CHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree){
	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = reinterpret_cast<D3DXMESHCONTAINER_DERIVED*>(pMeshContainerToFree);
	
	SafeRelease(pMeshContainer->pOriginalMesh);

	SafeDelete_Array(pMeshContainer->pRenderingMatrices);
	SafeDelete_Array(pMeshContainer->ppCombinedTransformationMatrices);
	SafeDelete_Array(pMeshContainer->pOffsetMatrices);

	SafeRelease(pMeshContainer->pSkinInfo);

	SafeDelete_Array(pMeshContainer->pAdjacency);

	for(_uint i = 0; i< pMeshContainer->NumMaterials; ++i)
		for(_uint j = 0; j < MESH_TEXTURE::TYPE_END; ++j){
			SafeRelease(pMeshContainer->pMeshTexture[i].pMeshTexture[j]);
		}

	SafeDelete_Array(pMeshContainer->pMeshTexture);

	SafeDelete_Array(pMeshContainer->pMaterials);

	SafeRelease(pMeshContainer->MeshData.pMesh);

	SafeDelete_Array(pMeshContainer->Name);

	SafeDelete(pMeshContainer);

	return NOERROR;
}

HRESULT CHierarchyLoader::AllocCopyName(char ** _ppAllocStirng, const char * _szOriginString){
	_uint uiLength = strlen(_szOriginString);

	*_ppAllocStirng = new char[uiLength + 1];
	::ZeroMemory(*_ppAllocStirng, uiLength + 1);

	::strcpy_s(*_ppAllocStirng, uiLength + 1, _szOriginString);

	return NOERROR;
}


HRESULT CHierarchyLoader::ChangeTextureFileName(_tchar * _szOut, const _tchar * _szIn, const _tchar * _szSrcText, const _tchar * _szDestText){
	if(nullptr == _szOut
		|| nullptr == _szIn
		|| nullptr == _szSrcText
		|| nullptr == _szDestText)
		return E_FAIL;

	_uint uiLength = lstrlen(_szIn);

	lstrcpy(_szOut, _szIn);

	if(!lstrcmp(&(_szIn[uiLength - 7]), _szSrcText)){
		lstrcpy(&(_szOut[uiLength - 7]), _szDestText);
	}

	return NOERROR;
}

CHierarchyLoader * CHierarchyLoader::CreateHierachyLoader(LPDIRECT3DDEVICE9 _pGraphicDevice, const _tchar* _szMeshPath){
	CHierarchyLoader* pInstance = new CHierarchyLoader(_pGraphicDevice);
	if(FAILED(pInstance->InitHierarchyLoader(_szMeshPath))){
		SafeRelease(pInstance);
		MSG_BOX(L"CHierarchyLoader Create Failure");
	}

	return pInstance;
}

void CHierarchyLoader::Free(){
	SafeRelease(m_pGraphicDevice);
}