#include "..\Header\DynamicMesh.h"


// 생성자
CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pRootFrame(nullptr)
	, m_pHierachyLoader(nullptr)
	, m_pAnimationController(nullptr){
	::ZeroMemory(&m_LocalMatrix,sizeof(_matrix));
}

// 복사 생성자
CDynamicMesh::CDynamicMesh(const CDynamicMesh & _DynamicMesh)
	: CComponent(_DynamicMesh)
	, m_pRootFrame(_DynamicMesh.m_pRootFrame)
	, m_pHierachyLoader(_DynamicMesh.m_pHierachyLoader)
	, m_pAnimationController(_DynamicMesh.m_pAnimationController->CloneAnimationController())
	, m_vecMeshContainer(_DynamicMesh.m_vecMeshContainer)
	, m_LocalMatrix(_DynamicMesh.m_LocalMatrix){
	SafeAddRef(m_pHierachyLoader);
}

// Initialize with Prototype
HRESULT CDynamicMesh::InitComponent_Proto(const _tchar * _szMeshPath, const _tchar * _szMeshName, const _matrix * _pLocalTransMatrix){
	_tchar szFilePath[MAX_PATH] = L"";
	::lstrcpy(szFilePath, _szMeshPath);
	::lstrcat(szFilePath, _szMeshName);

	// ID3DXAllocateHierarchy의 구현체를 준비함.
	m_pHierachyLoader = CHierarchyLoader::CreateHierachyLoader(CComponent::GetGraphicDevice(), _szMeshPath);
	if(nullptr == m_pHierachyLoader)
		return E_FAIL;

	// 애니메이션 컨트롤러를 받을 변수
	LPD3DXANIMATIONCONTROLLER pAniCtrl = nullptr;

	// 다이나믹 메쉬를 Hierarchy를 통해 로드.
	if(FAILED(D3DXLoadMeshHierarchyFromX(szFilePath, D3DXMESH_MANAGED, CComponent::GetGraphicDevice(), m_pHierachyLoader, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;
	
	// 애니메이션 컨트롤러를 관리하는 클래스를 생성
	m_pAnimationController = CAnimationController::CreateAnimationController(pAniCtrl);
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	// 로컬 변환 행렬을 받아오지 않았다면 항등행렬을 만듬
	if(nullptr != _pLocalTransMatrix){
		m_LocalMatrix = *_pLocalTransMatrix;
	} else
		D3DXMatrixIdentity(&m_LocalMatrix);

	// 로컬 변환 행렬을 이용하여 본들의 변환 행렬을 다시 계산함
	UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(m_pRootFrame), m_LocalMatrix);

	// 각 뼈들의 변환 행렬을 
	if(FAILED(SetUpCombinedTransformationMatrixPointer(reinterpret_cast<D3DXFRAME_DERIVED*>(m_pRootFrame))))
		return E_FAIL;

	return NOERROR;
}

// Initialize with Clone
HRESULT CDynamicMesh::InitComponent_Clone(void * _pArgument){
	return NOERROR;
}

LPDIRECT3DTEXTURE9 CDynamicMesh::GetMaterialTexture(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex, MESH_TEXTURE::TEXTURE_TYPE _etype) const{
	if(m_vecMeshContainer.size() <= _uiMeshContainerIndex
		|| _uiSubsetIndex >= m_vecMeshContainer[_uiMeshContainerIndex]->NumMaterials
		|| nullptr == m_vecMeshContainer[_uiMeshContainerIndex]->pMeshTexture)
		return nullptr;
	
	return m_vecMeshContainer[_uiMeshContainerIndex]->pMeshTexture[_uiSubsetIndex][_etype];
}

_uint CDynamicMesh::GetCurrentAnaimationSet(){
	if(nullptr == m_pAnimationController)
		return 0;

	return m_pAnimationController->GetCurrentAnaimationSet();
}

// 메쉬 컨테이너의 스키닝 작업을 진행함
HRESULT CDynamicMesh::UpdateSkinnedMesh(_uint _uiMeshContainerIndex){
	if(m_vecMeshContainer.size() <= _uiMeshContainerIndex)
		return E_FAIL;

	// 렌더링에 필요한 행렬 계산(메쉬 컨테이너의 오프셋 행렬 * 메쉬에 영향을 끼치는 모든 뼈들의 변환행렬을 곱한 행렬)
	for(_ulong i = 0; i < m_vecMeshContainer[_uiMeshContainerIndex]->dwNumFrames; ++i)
		m_vecMeshContainer[_uiMeshContainerIndex]->pRenderingMatrices[i] = m_vecMeshContainer[_uiMeshContainerIndex]->pOffsetMatrices[i] * *(m_vecMeshContainer[_uiMeshContainerIndex]->ppCombinedTransformationMatrices[i]);

	void*	pVerticesSrc, *pVerticesDest;

	m_vecMeshContainer[_uiMeshContainerIndex]->pOriginalMesh->LockVertexBuffer(0, &pVerticesSrc);
	m_vecMeshContainer[_uiMeshContainerIndex]->MeshData.pMesh->LockVertexBuffer(0, &pVerticesDest);

	m_vecMeshContainer[_uiMeshContainerIndex]->pSkinInfo->UpdateSkinnedMesh(m_vecMeshContainer[_uiMeshContainerIndex]->pRenderingMatrices, nullptr, pVerticesSrc, pVerticesDest);

	m_vecMeshContainer[_uiMeshContainerIndex]->pOriginalMesh->UnlockVertexBuffer();
	m_vecMeshContainer[_uiMeshContainerIndex]->MeshData.pMesh->UnlockVertexBuffer();

	return NOERROR;
}

// 서브셋 단위로 렌더
HRESULT CDynamicMesh::RenderMesh(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex){
	if(m_vecMeshContainer.size() <= _uiMeshContainerIndex ||
		m_vecMeshContainer[_uiMeshContainerIndex]->NumMaterials <= _uiSubsetIndex)
		return E_FAIL;

	m_vecMeshContainer[_uiMeshContainerIndex]->MeshData.pMesh->DrawSubset(_uiSubsetIndex);
	
	return NOERROR;
}

// 인덱스에 해당하는 애니메이션을 설정함
HRESULT CDynamicMesh::SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed/* = 1.f*/){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	return m_pAnimationController->SetUpAnimationSet(_uiIndex, _fAnimationSpeed);
}

HRESULT CDynamicMesh::PlayAnimationSet(_double _dDeltaTime){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	m_pAnimationController->PlayAnimationSet(_dDeltaTime);

	return UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(m_pRootFrame), m_LocalMatrix);
}

// 현재 재생중인 애니메이션이 끝났는지 확인
_bool CDynamicMesh::IsFinished(){
	if(nullptr == m_pAnimationController)
		return false;

	return m_pAnimationController->IsFinished();
}

// 모든 본들의 변환행렬을 업데이트 함(부모뼈의 영향까지 계산하기 위해 부모뼈의 매트릭스와 곱함)
HRESULT CDynamicMesh::UpdateCombinedTransformationMatrix(D3DXFRAME_DERIVED * _pFrame, const _matrix & _ParentMatrix){
	_pFrame->CombinedTransformationMatrix = _pFrame->TransformationMatrix * _ParentMatrix;
	
	if(nullptr != _pFrame->pFrameFirstChild)
		UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameFirstChild), _pFrame->CombinedTransformationMatrix);

	if(nullptr != _pFrame->pFrameSibling)
		UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameSibling), _ParentMatrix);
	
	return NOERROR;
}

// 메쉬 컨테이너가 가지는 모든 본의 변환 행렬을 메쉬 컨테이너에서 보관하도록 세팅함
HRESULT CDynamicMesh::SetUpCombinedTransformationMatrixPointer(D3DXFRAME_DERIVED * _pFrame){
	if(nullptr != _pFrame->pMeshContainer){
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = reinterpret_cast<D3DXMESHCONTAINER_DERIVED*>(_pFrame->pMeshContainer);

		for(_uint i = 0; i < pMeshContainer->dwNumFrames; ++i){
			D3DXFRAME_DERIVED* pFindFrame = reinterpret_cast<D3DXFRAME_DERIVED*>(D3DXFrameFind(m_pRootFrame, pMeshContainer->pSkinInfo->GetBoneName(i)));

			// 메쉬 컨테이너에 뼈들의 ???를 담는다.
			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFindFrame->CombinedTransformationMatrix;
		}

		m_vecMeshContainer.push_back(pMeshContainer);
	}

	if(nullptr != _pFrame->pFrameFirstChild)
		SetUpCombinedTransformationMatrixPointer(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameFirstChild));

	if(nullptr != _pFrame->pFrameSibling)
		SetUpCombinedTransformationMatrixPointer(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameSibling));
	
	return NOERROR;
}

// 원본생성 함수
CDynamicMesh * CDynamicMesh::CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar * _szMeshPath, const _tchar * _szMeshName, const _matrix * _pLocalTransMatrix){
	CDynamicMesh* pInstance = new CDynamicMesh(_pGrahpicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szMeshPath, _szMeshName, _pLocalTransMatrix))){
		SafeRelease(pInstance);
		MSG_BOX(L"CDynamicMesh Create Failure");
	}
	
	return pInstance;
}

// 복제 생성 함수
CComponent * CDynamicMesh::CreateClone(void * _pArgument){
	CComponent* pInstance = new CDynamicMesh(*this);
	if(FAILED(pInstance->InitComponent_Clone(_pArgument))){
		SafeRelease(pInstance);
		MSG_BOX(L"CDynamicMesh Clone Failure");
	}

	return pInstance;
}

// Free
void CDynamicMesh::Free(){
	if(!IsClone()){
		if(nullptr != m_pRootFrame)
			m_pHierachyLoader->DestroyFrame(m_pRootFrame);
	}
	SafeRelease(m_pHierachyLoader);
	SafeRelease(m_pAnimationController);

	CComponent::Free();
}
