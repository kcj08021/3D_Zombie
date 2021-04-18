#include "..\Header\DynamicMesh.h"


// ������
CDynamicMesh::CDynamicMesh(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pRootFrame(nullptr)
	, m_pHierachyLoader(nullptr)
	, m_pAnimationController(nullptr){
	::ZeroMemory(&m_LocalMatrix,sizeof(_matrix));
}

// ���� ������
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

	// ID3DXAllocateHierarchy�� ����ü�� �غ���.
	m_pHierachyLoader = CHierarchyLoader::CreateHierachyLoader(CComponent::GetGraphicDevice(), _szMeshPath);
	if(nullptr == m_pHierachyLoader)
		return E_FAIL;

	// �ִϸ��̼� ��Ʈ�ѷ��� ���� ����
	LPD3DXANIMATIONCONTROLLER pAniCtrl = nullptr;

	// ���̳��� �޽��� Hierarchy�� ���� �ε�.
	if(FAILED(D3DXLoadMeshHierarchyFromX(szFilePath, D3DXMESH_MANAGED, CComponent::GetGraphicDevice(), m_pHierachyLoader, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;
	
	// �ִϸ��̼� ��Ʈ�ѷ��� �����ϴ� Ŭ������ ����
	m_pAnimationController = CAnimationController::CreateAnimationController(pAniCtrl);
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	// ���� ��ȯ ����� �޾ƿ��� �ʾҴٸ� �׵������ ����
	if(nullptr != _pLocalTransMatrix){
		m_LocalMatrix = *_pLocalTransMatrix;
	} else
		D3DXMatrixIdentity(&m_LocalMatrix);

	// ���� ��ȯ ����� �̿��Ͽ� ������ ��ȯ ����� �ٽ� �����
	UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(m_pRootFrame), m_LocalMatrix);

	// �� ������ ��ȯ ����� 
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

// �޽� �����̳��� ��Ű�� �۾��� ������
HRESULT CDynamicMesh::UpdateSkinnedMesh(_uint _uiMeshContainerIndex){
	if(m_vecMeshContainer.size() <= _uiMeshContainerIndex)
		return E_FAIL;

	// �������� �ʿ��� ��� ���(�޽� �����̳��� ������ ��� * �޽��� ������ ��ġ�� ��� ������ ��ȯ����� ���� ���)
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

// ����� ������ ����
HRESULT CDynamicMesh::RenderMesh(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex){
	if(m_vecMeshContainer.size() <= _uiMeshContainerIndex ||
		m_vecMeshContainer[_uiMeshContainerIndex]->NumMaterials <= _uiSubsetIndex)
		return E_FAIL;

	m_vecMeshContainer[_uiMeshContainerIndex]->MeshData.pMesh->DrawSubset(_uiSubsetIndex);
	
	return NOERROR;
}

// �ε����� �ش��ϴ� �ִϸ��̼��� ������
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

// ���� ������� �ִϸ��̼��� �������� Ȯ��
_bool CDynamicMesh::IsFinished(){
	if(nullptr == m_pAnimationController)
		return false;

	return m_pAnimationController->IsFinished();
}

// ��� ������ ��ȯ����� ������Ʈ ��(�θ���� ������� ����ϱ� ���� �θ���� ��Ʈ������ ����)
HRESULT CDynamicMesh::UpdateCombinedTransformationMatrix(D3DXFRAME_DERIVED * _pFrame, const _matrix & _ParentMatrix){
	_pFrame->CombinedTransformationMatrix = _pFrame->TransformationMatrix * _ParentMatrix;
	
	if(nullptr != _pFrame->pFrameFirstChild)
		UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameFirstChild), _pFrame->CombinedTransformationMatrix);

	if(nullptr != _pFrame->pFrameSibling)
		UpdateCombinedTransformationMatrix(reinterpret_cast<D3DXFRAME_DERIVED*>(_pFrame->pFrameSibling), _ParentMatrix);
	
	return NOERROR;
}

// �޽� �����̳ʰ� ������ ��� ���� ��ȯ ����� �޽� �����̳ʿ��� �����ϵ��� ������
HRESULT CDynamicMesh::SetUpCombinedTransformationMatrixPointer(D3DXFRAME_DERIVED * _pFrame){
	if(nullptr != _pFrame->pMeshContainer){
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = reinterpret_cast<D3DXMESHCONTAINER_DERIVED*>(_pFrame->pMeshContainer);

		for(_uint i = 0; i < pMeshContainer->dwNumFrames; ++i){
			D3DXFRAME_DERIVED* pFindFrame = reinterpret_cast<D3DXFRAME_DERIVED*>(D3DXFrameFind(m_pRootFrame, pMeshContainer->pSkinInfo->GetBoneName(i)));

			// �޽� �����̳ʿ� ������ ???�� ��´�.
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

// �������� �Լ�
CDynamicMesh * CDynamicMesh::CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar * _szMeshPath, const _tchar * _szMeshName, const _matrix * _pLocalTransMatrix){
	CDynamicMesh* pInstance = new CDynamicMesh(_pGrahpicDevice);
	if(FAILED(pInstance->InitComponent_Proto(_szMeshPath, _szMeshName, _pLocalTransMatrix))){
		SafeRelease(pInstance);
		MSG_BOX(L"CDynamicMesh Create Failure");
	}
	
	return pInstance;
}

// ���� ���� �Լ�
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
