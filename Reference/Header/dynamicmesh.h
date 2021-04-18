#ifndef __DYNAMIC_MESH_H__
#define __DYNAMIC_MESH_H__

#include "Component.h"
#include "..\Header\HierarchyLoader.h"
#include "..\Header\AnimationController.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh final:
	public CComponent{
private:
	// 생성자
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// 복사 생성자
	explicit CDynamicMesh(const CDynamicMesh& _DynamicMesh);
	// 소멸자
	virtual ~CDynamicMesh() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(const _tchar* _szMeshPath, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;	
	
	// Get
public:
	// 메쉬 컨테이너의 갯수를 반환
	_uint GetMeshContainerCount() const{
		return m_vecMeshContainer.size();
	}
	// 특정 메쉬 컨테이너의 서브셋 갯수를 얻어옴
	_uint GetSubsetCount(_uint _uiMeshContainerIndex) const{
		return m_vecMeshContainer[_uiMeshContainerIndex]->NumMaterials;
	}
	// 특정 프레임을 검색하여 얻어옴
	D3DXFRAME_DERIVED* GetFrame(const char* _szFrameName){
		return reinterpret_cast<D3DXFRAME_DERIVED*>(D3DXFrameFind(m_pRootFrame, _szFrameName));
	}
	// 메쉬컨테이너의 특정 서브셋의 재질 텍스쳐를 얻어옴
	LPDIRECT3DTEXTURE9 GetMaterialTexture(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex, MESH_TEXTURE::TEXTURE_TYPE eType) const;


public:
	// 현재 재생중인 애니메이션 세트의 인덱스를 가져옴
	_uint GetCurrentAnaimationSet();

public:
	// 메쉬 컨테이너의 스키닝 작업을 진행함
	HRESULT UpdateSkinnedMesh(_uint _uiMeshContainerIndex);
	// 서브셋 단위로 렌더
	HRESULT RenderMesh(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex);
	
public:
	// 인덱스에 해당하는 애니메이션을 설정함
	HRESULT SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed = 1.f);
	// 현재 세팅된 애니메이션을 재생함
	HRESULT PlayAnimationSet(_double _dDeltaTime);
	// 현재 재생중인 애니메이션이 끝났는지 확인함
	_bool IsFinished();
	// 현재 재생중인 트랙의 누적 재생시간을 초기화함
	void ResetTime(){
		m_pAnimationController->ResetTime();
	}

private:
	// 가장 최상위의 뼈를 가리키는 포인터
	D3DXFRAME* m_pRootFrame = nullptr;
	// 다이나믹 메쉬를 로드하기 위한 Hirerachy 클래스
	CHierarchyLoader* m_pHierachyLoader = nullptr;
	// 애니메이션 컨트롤러
	CAnimationController* m_pAnimationController = nullptr;
	// 메쉬컨테이너를 보관하는 컨테이너(vector)
	vector<D3DXMESHCONTAINER_DERIVED*> m_vecMeshContainer;

	// 메쉬를 로컬상에서 변환하는 행렬
	_matrix m_LocalMatrix;

private:
	// 모든 본들의 변환행렬을 업데이트 함
	HRESULT UpdateCombinedTransformationMatrix(D3DXFRAME_DERIVED* _pFrame, const _matrix& _ParentMatrix);
	// 메쉬 컨테이너가 가지는 모든 본의 변환 행렬을 메쉬 컨테이너에서 보관하도록 세팅함
	HRESULT SetUpCombinedTransformationMatrixPointer(D3DXFRAME_DERIVED* _pFrame);

public:
	// 원본생성 함수
	static CDynamicMesh* CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar* _szMeshPath, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix = nullptr);
	// 복제 생성 함수
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};

END

#endif // !__DYNAMIC_MESH_H__
