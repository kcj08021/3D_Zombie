#include "..\Header\HierarchyLoader.h"

// ������
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

// ������ ����
STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR _Name, LPD3DXFRAME * _ppNewFrame){
	// D3DXFRAME�� ��ӹ޾� ���� ����ü��
	D3DXFRAME_DERIVED* pFrameDrived = new D3DXFRAME_DERIVED;
	::ZeroMemory(pFrameDrived, sizeof(D3DXFRAME_DERIVED));

	// �������� �̸��� ����
	if(nullptr != _Name)
		AllocCopyName(&pFrameDrived->Name, _Name);

	// �� �������� ��ȯ ����� �̸� �׵���ķ� �����, ���� ��ӹ޾� ������ ����� CombinedTransformationMatrix�� �׵���ķ� ����
	pFrameDrived->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pFrameDrived->TransformationMatrix);

	// ������� �������� ���ڷ� ���� ������ �־���(���)
	*_ppNewFrame = pFrameDrived;

	return NOERROR;
}

// �޽� �����̳� ����
STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR _Name, CONST D3DXMESHDATA * _pMeshData, CONST D3DXMATERIAL * _pMaterials, CONST D3DXEFFECTINSTANCE * _pEffectInstances, DWORD _NumMaterials, CONST DWORD * _pAdjacency, LPD3DXSKININFO _pSkinInfo, LPD3DXMESHCONTAINER * _ppNewMeshContainer){
	D3DXMESHCONTAINER_DERIVED* pMeshContainerDrived = new D3DXMESHCONTAINER_DERIVED;
	::ZeroMemory(pMeshContainerDrived, sizeof(D3DXMESHCONTAINER_DERIVED));

	// �޽� �����̳��� �̸��� �Ҵ��Ͽ� ����
	if(nullptr != _Name)
		AllocCopyName(&pMeshContainerDrived->Name, _Name);

	// �޽� Ÿ��(�޽� Ÿ���� �޽�)
	pMeshContainerDrived->MeshData.Type = D3DXMESHTYPE_MESH;

	// �޽� ������ ������� ���� ������ �޾ƿ�
	LPD3DXMESH pMesh = _pMeshData->pMesh;
	SafeAddRef(pMesh);

	//// FVF�� ���缭 �޽��� ������
	//if(FAILED(pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() /*| D3DFVF_NORMAL*/, m_pGraphicDevice, &(pMeshContainerDrived->MeshData.pMesh))))
	//	return E_FAIL;

	// �޽��� ���� ������ ������� ����ü
	D3DVERTEXELEMENT9 Decl[MAX_FVF_DECL_SIZE]; /*= {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};*/

	// �޽��� ���� ���� ������
	pMesh->GetDeclaration(Decl);

	// ������ �޽� ������ �޽��� ������
	if(FAILED(pMesh->CloneMesh(pMesh->GetOptions(), Decl, m_pGraphicDevice, &pMeshContainerDrived->MeshData.pMesh)))
		return E_FAIL;
	
	SafeRelease(pMesh);

	// �޽� �����̳ʿ� ���� ���� ����
	pMeshContainerDrived->NumMaterials = _NumMaterials;
	
	// �޽� �����̳ʿ� ���� ����
	pMeshContainerDrived->pMaterials = new D3DXMATERIAL[_NumMaterials];
	::memcpy(pMeshContainerDrived->pMaterials, _pMaterials, sizeof(D3DXMATERIAL)* _NumMaterials);

	// �޽� �����̳ʿ� ��ӹ޾� ���� ����� �޽� �ؽ����� ������ �Ҵ�
	pMeshContainerDrived->pMeshTexture = new MESH_TEXTURE[pMeshContainerDrived->NumMaterials];
	::ZeroMemory(pMeshContainerDrived->pMeshTexture, sizeof(MESH_TEXTURE) * pMeshContainerDrived->NumMaterials);

	_tchar szFilePath[MAX_PATH] = L"";

	// �޽� �����̳ʿ� ��ο� ����ִ� �ؽ��� �ε��Ͽ� ������
	for(_uint i = 0; i < pMeshContainerDrived->NumMaterials; ++i){
		// �ؽ��� ������ ���� ��� ����
		::lstrcpy(szFilePath, m_szMeshPath);

		// ���� ������ ����ִ� �ؽ��� ������ �̸��� ������(��Ƽ����Ʈ->���̵����Ʈ�� ��ȯ)
		_tchar szFileName[MAX_PATH] = L"";

		::MultiByteToWideChar(CP_ACP, 0, pMeshContainerDrived->pMaterials[i].pTextureFilename, strlen(pMeshContainerDrived->pMaterials[i].pTextureFilename), szFileName, MAX_PATH);

		// ���� ��ο� ���ļ� ���� ������ ��ġ�� ����
		::lstrcat(szFilePath, szFileName);

		_tchar		szOut[MAX_PATH] = L"";

		// ��ǻ�� �ؽ��� �ε�
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"col.dds");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_DIFFUSE]);

		// �븻�� �ؽ��� �ε�
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

		// ����ŧ���� �ؽ��� �ε�
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"spc.tga");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_SPECULAR]);

		// �̹̽ú�� �ؽ��� �ε�
		ChangeTextureFileName(szOut, szFilePath, L"col.dds", L"emi");
		D3DXCreateTextureFromFile(m_pGraphicDevice, szOut, &pMeshContainerDrived->pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_EMISSIVE]);
	}

	// �޽� �����̳ʿ� ������� ����
	pMeshContainerDrived->pAdjacency = new _ulong[pMeshContainerDrived->MeshData.pMesh->GetNumFaces() * 3];
	memcpy(pMeshContainerDrived->pAdjacency, _pAdjacency, sizeof(_ulong) * pMeshContainerDrived->MeshData.pMesh->GetNumFaces() * 3);

	// �޽� �����̳ʿ� ��Ų���� ����
	pMeshContainerDrived->pSkinInfo = _pSkinInfo;
	SafeAddRef(pMeshContainerDrived->pSkinInfo);

	// �޽� �����̳ʿ� �������� ���� ����
	pMeshContainerDrived->dwNumFrames = pMeshContainerDrived->pSkinInfo->GetNumBones();

	// �޽� �����̳ʿ� ������ ����� �迭�� �����, ����
	/* ���� �޽��� ������ ��ġ���� �ʱ� ������ �������� �ʿ��� */
	pMeshContainerDrived->pOffsetMatrices = new _matrix[pMeshContainerDrived->dwNumFrames];
	for(_ulong i = 0; i < pMeshContainerDrived->dwNumFrames; ++i){
		pMeshContainerDrived->pOffsetMatrices[i] = *pMeshContainerDrived->pSkinInfo->GetBoneOffsetMatrix(i);
	}

	// �θ� ������ ������ ��� �����͸� �迭�� �̸� �Ҵ��ص�
	/* �޽� �����̳ʸ� ���� ���Ŀ� ���� ä����� ���� */
	pMeshContainerDrived->ppCombinedTransformationMatrices = new _matrix*[pMeshContainerDrived->dwNumFrames];
	
	// �����ÿ� �̿�Ǵ� ����� ������ �迭�� �̸� �Ҵ��ص�
	/* �����ÿ� ���Ǵ� ����̱⿡ ���� ���� ä����� ���� */
	pMeshContainerDrived->pRenderingMatrices = new _matrix[pMeshContainerDrived->dwNumFrames];
	
	// ���� �ʱ������ �޽��� ���� �޽� �����̳ʿ��� ������
	/* �� �޽��� ����� ������ �ʰ� �� */
	if(FAILED(pMeshContainerDrived->MeshData.pMesh->CloneMesh(pMeshContainerDrived->MeshData.pMesh->GetOptions(), Decl, m_pGraphicDevice, &pMeshContainerDrived->pOriginalMesh)))
		return E_FAIL;

	*_ppNewMeshContainer = pMeshContainerDrived;

	return NOERROR;
}

// �ҷ��� �������� �Ҵ� ����
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

// �޽� �����̳� �Ҵ� ����
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