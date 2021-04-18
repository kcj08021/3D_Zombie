#include "..\Header\StaticMesh.h"

CStaticMesh::CStaticMesh(LPDIRECT3DDEVICE9 _pGrahpicDevice)
	: CComponent(_pGrahpicDevice)
	, m_pStaticMesh(nullptr)
	, m_pAdjacencyBuffer(nullptr)
	, m_pMaterialBuffer(nullptr)
	, m_dwMaterialCount(0)
	, m_pMaterial(nullptr)
	, m_pMeshTexture(nullptr)
	, m_uiStride(0){
}

CStaticMesh::CStaticMesh(const CStaticMesh & _StaticMesh)
	: CComponent(_StaticMesh)
	, m_pStaticMesh(_StaticMesh.m_pStaticMesh)
	, m_pAdjacencyBuffer(_StaticMesh.m_pAdjacencyBuffer)
	, m_pMaterialBuffer(_StaticMesh.m_pMaterialBuffer)
	, m_dwMaterialCount(_StaticMesh.m_dwMaterialCount)
	, m_pMaterial(_StaticMesh.m_pMaterial)
	, m_pMeshTexture(_StaticMesh.m_pMeshTexture)
	, m_uiStride(_StaticMesh.m_uiStride){
	SafeAddRef(m_pStaticMesh);
	SafeAddRef(m_pAdjacencyBuffer);
	SafeAddRef(m_pMaterialBuffer);
	for(size_t i =0 ; i< m_dwMaterialCount; ++i)
		for(size_t j = 0; j < MESH_TEXTURE::TYPE_END; ++j){
			SafeAddRef(m_pMeshTexture[i].pMeshTexture[j]);
		}
}

HRESULT CStaticMesh::InitComponent_Proto(const _tchar * _szMeshPath, const _tchar * _szMeshName, const _matrix* _pLocalTransMatrix){
	_tchar szFilePath[MAX_PATH] = L"";
	::lstrcpy(szFilePath, _szMeshPath);
	::lstrcat(szFilePath, _szMeshName);

	if(FAILED(D3DXLoadMeshFromX(szFilePath, D3DXMESH_MANAGED, CComponent::GetGraphicDevice(), &m_pAdjacencyBuffer, &m_pMaterialBuffer, nullptr, &m_dwMaterialCount, &m_pStaticMesh))){
		_tchar szError[MAX_PATH * 2] = L"StaticMesh Load Failure: \n";
		::lstrcat(szError, szFilePath);
		MSG_BOX(szError);
		return E_FAIL;
	}

	// 재질 정보를 동적할당 한 뒤 버퍼에서 정보 꺼내옴
	m_pMaterial = new D3DXMATERIAL[m_dwMaterialCount];
	::memcpy(m_pMaterial, m_pMaterialBuffer->GetBufferPointer(), sizeof(D3DXMATERIAL) * m_dwMaterialCount);

	// 메쉬에 사용될 텍스쳐 변수를 모아놓은 구조체 동적 할당
	m_pMeshTexture = new MESH_TEXTURE[m_dwMaterialCount];
	::ZeroMemory(m_pMeshTexture, sizeof(MESH_TEXTURE) * m_dwMaterialCount);

	// 경로에 들어있는 텍스쳐를 가져옴
	for(_uint i = 0; i < m_dwMaterialCount; ++i){
		// 텍스쳐 파일의 폴더 경로 설정
		::lstrcpy(szFilePath, _szMeshPath);

		// 재질 정보에 들어있는 텍스쳐 파일의 이름을 가져옴(멀티바이트->와이드바이트로 변환)
		_tchar szFileName[MAX_PATH] = L"";
		if(nullptr == m_pMaterial[i].pTextureFilename)
			continue;

		::MultiByteToWideChar(CP_ACP, 0, m_pMaterial[i].pTextureFilename, strlen(m_pMaterial[i].pTextureFilename), szFileName, MAX_PATH);

		// 폴더 경로와 합쳐서 실제 파일의 위치로 만듬
		::lstrcat(szFilePath, szFileName);

		_tchar		szOut[MAX_PATH] = L"";

		// 디퓨즈 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col", L"col");
		D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szOut, &m_pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_DIFFUSE]);
		
		// 노말맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col", L"nml");

		LPDIRECT3DTEXTURE9 pHieghtTexture = nullptr;
		D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szOut, &pHieghtTexture);
		if(nullptr != pHieghtTexture){
			D3DSURFACE_DESC desc;
			pHieghtTexture->GetLevelDesc(0, &desc);
			D3DXCreateTexture(CComponent::GetGraphicDevice(), desc.Width, desc.Height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_NORMAL]);
			D3DXComputeNormalMap(m_pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_NORMAL], pHieghtTexture, nullptr, 0, D3DX_CHANNEL_RED, 5.f);
			SafeRelease(pHieghtTexture);
		}
	
		// 스펙큘러맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"col", L"spc");
		D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szOut, &m_pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_SPECULAR]);

		// 이미시브맵 텍스쳐 로드
		ChangeTextureFileName(szOut, szFilePath, L"D", L"E");
		D3DXCreateTextureFromFile(CComponent::GetGraphicDevice(), szOut, &m_pMeshTexture[i].pMeshTexture[MESH_TEXTURE::TYPE_EMISSIVE]);
	}

	// 변환행렬을 받아온경우 변환행렬로 메쉬 초기화
	D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
	m_pStaticMesh->GetDeclaration(decl);

	_ulong dwVerticesCount = m_pStaticMesh->GetNumVertices();
	_uint uiNormalOffset = 0;
	
	for(_uint i = 0; i < MAX_FVF_DECL_SIZE; ++i){
		if(decl[i].Usage == D3DDECLUSAGE_NORMAL){
			uiNormalOffset = decl[i].Offset;
		}
		if(decl[i].Stream == 0xff){
			m_uiStride = decl[i-1].Offset + 8;
			break;
		}
	}

	_matrix matlocal = {};
	D3DXMatrixIdentity(&matlocal);
	if(nullptr != _pLocalTransMatrix){
		matlocal = *_pLocalTransMatrix;
	}

	void* pVertices = nullptr;
	m_pStaticMesh->LockVertexBuffer(0, &pVertices);

	_vec3 vec;
	for(_ulong i = 0; i < dwVerticesCount; ++i){
		// 위치 변환
		vec = *D3DXVec3TransformCoord(reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + (i * m_uiStride)), reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + (i * m_uiStride)), &matlocal);
		// 노말값 변환
		D3DXVec3TransformNormal(reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + uiNormalOffset + (i * m_uiStride)), reinterpret_cast<_vec3*>(reinterpret_cast<_byte*>(pVertices) + uiNormalOffset + (i * m_uiStride)), &matlocal);
	}

	m_pStaticMesh->UnlockVertexBuffer();
	return NOERROR;
}

HRESULT CStaticMesh::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

LPDIRECT3DTEXTURE9 CStaticMesh::GetMaterialTexture(_uint _uiSubsetIndex, MESH_TEXTURE::TEXTURE_TYPE eType) const{
	if(_uiSubsetIndex >= m_dwMaterialCount
		|| nullptr == m_pMeshTexture){
		return nullptr;
	}	

	return m_pMeshTexture[_uiSubsetIndex][eType];
}

HRESULT CStaticMesh::GetBoundaryBox(_vec3& _vMin, _vec3& _vMax){
	if(nullptr == m_pStaticMesh)
		return E_FAIL;

	void* pVertices = nullptr;

	m_pStaticMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertices);

	if(FAILED(D3DXComputeBoundingBox(reinterpret_cast<_vec3*>(pVertices), m_pStaticMesh->GetNumVertices(), m_uiStride, &_vMin, &_vMax))){
		m_pStaticMesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	m_pStaticMesh->UnlockVertexBuffer();
		
	return NOERROR;
}

HRESULT CStaticMesh::GetBoundarySphere(_vec3 & _vCenter, _float _fRadius){
	if(nullptr == m_pStaticMesh)
		return E_FAIL;

	void* pVertices = nullptr;

	m_pStaticMesh->LockVertexBuffer(D3DLOCK_READONLY, &pVertices);

	if(FAILED(D3DXComputeBoundingSphere(reinterpret_cast<_vec3*>(pVertices), m_pStaticMesh->GetNumVertices(), m_uiStride, &_vCenter, &_fRadius))){
		m_pStaticMesh->UnlockVertexBuffer();
		return E_FAIL;
	}

	m_pStaticMesh->UnlockVertexBuffer();

	return NOERROR;
}

HRESULT CStaticMesh::RenderMesh(_uint _uiSubsetIndex){
	if(_uiSubsetIndex >= m_dwMaterialCount
		|| nullptr == m_pStaticMesh){
		return E_FAIL;
	}

	m_pStaticMesh->DrawSubset(_uiSubsetIndex);

	return NOERROR;
}

HRESULT CStaticMesh::ChangeTextureFileName(_tchar * _szOut, const _tchar * _szIn, const _tchar * _szSrcText, const _tchar * _szDestText){
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

CStaticMesh * CStaticMesh::CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar * _szMeshDir, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix/* = nullptr*/){
	CStaticMesh* pInstance = new CStaticMesh(_pGrahpicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szMeshDir, _szMeshName, _pLocalTransMatrix))){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMesh Create Failure");
	}
	
	return pInstance;
}

CComponent * CStaticMesh::CreateClone(void* _pArgument){
	CStaticMesh* pInstance = new CStaticMesh(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CStaticMesh Clone Failure");
	}

	return pInstance;
}

void CStaticMesh::Free(){
	for(size_t i = 0; i< m_dwMaterialCount; ++i)
		for(size_t j = 0; j < MESH_TEXTURE::TYPE_END; ++j){
			SafeRelease(m_pMeshTexture[i].pMeshTexture[j]);
		}

	if(!CComponent::IsClone()){
		SafeDelete_Array(m_pMaterial);
		SafeDelete_Array(m_pMeshTexture);
	}

	SafeRelease(m_pStaticMesh);
	SafeRelease(m_pAdjacencyBuffer);
	SafeRelease(m_pMaterialBuffer);

	CComponent::Free();
}
