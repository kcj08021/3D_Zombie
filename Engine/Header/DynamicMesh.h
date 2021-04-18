#ifndef __DYNAMIC_MESH_H__
#define __DYNAMIC_MESH_H__

#include "Component.h"
#include "..\Header\HierarchyLoader.h"
#include "..\Header\AnimationController.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh final:
	public CComponent{
private:
	// ������
	explicit CDynamicMesh(LPDIRECT3DDEVICE9 _pGraphicDevice);
	// ���� ������
	explicit CDynamicMesh(const CDynamicMesh& _DynamicMesh);
	// �Ҹ���
	virtual ~CDynamicMesh() = default;

private:
	// Initialize with Prototype
	virtual HRESULT InitComponent_Proto(const _tchar* _szMeshPath, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix);
	// Initialize with Clone
	virtual HRESULT InitComponent_Clone(void* _pArgument) override;	
	
	// Get
public:
	// �޽� �����̳��� ������ ��ȯ
	_uint GetMeshContainerCount() const{
		return m_vecMeshContainer.size();
	}
	// Ư�� �޽� �����̳��� ����� ������ ����
	_uint GetSubsetCount(_uint _uiMeshContainerIndex) const{
		return m_vecMeshContainer[_uiMeshContainerIndex]->NumMaterials;
	}
	// Ư�� �������� �˻��Ͽ� ����
	D3DXFRAME_DERIVED* GetFrame(const char* _szFrameName){
		return reinterpret_cast<D3DXFRAME_DERIVED*>(D3DXFrameFind(m_pRootFrame, _szFrameName));
	}
	// �޽������̳��� Ư�� ������� ���� �ؽ��ĸ� ����
	LPDIRECT3DTEXTURE9 GetMaterialTexture(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex, MESH_TEXTURE::TEXTURE_TYPE eType) const;


public:
	// ���� ������� �ִϸ��̼� ��Ʈ�� �ε����� ������
	_uint GetCurrentAnaimationSet();

public:
	// �޽� �����̳��� ��Ű�� �۾��� ������
	HRESULT UpdateSkinnedMesh(_uint _uiMeshContainerIndex);
	// ����� ������ ����
	HRESULT RenderMesh(_uint _uiMeshContainerIndex, _uint _uiSubsetIndex);
	
public:
	// �ε����� �ش��ϴ� �ִϸ��̼��� ������
	HRESULT SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed = 1.f);
	// ���� ���õ� �ִϸ��̼��� �����
	HRESULT PlayAnimationSet(_double _dDeltaTime);
	// ���� ������� �ִϸ��̼��� �������� Ȯ����
	_bool IsFinished();
	// ���� ������� Ʈ���� ���� ����ð��� �ʱ�ȭ��
	void ResetTime(){
		m_pAnimationController->ResetTime();
	}

private:
	// ���� �ֻ����� ���� ����Ű�� ������
	D3DXFRAME* m_pRootFrame = nullptr;
	// ���̳��� �޽��� �ε��ϱ� ���� Hirerachy Ŭ����
	CHierarchyLoader* m_pHierachyLoader = nullptr;
	// �ִϸ��̼� ��Ʈ�ѷ�
	CAnimationController* m_pAnimationController = nullptr;
	// �޽������̳ʸ� �����ϴ� �����̳�(vector)
	vector<D3DXMESHCONTAINER_DERIVED*> m_vecMeshContainer;

	// �޽��� ���û󿡼� ��ȯ�ϴ� ���
	_matrix m_LocalMatrix;

private:
	// ��� ������ ��ȯ����� ������Ʈ ��
	HRESULT UpdateCombinedTransformationMatrix(D3DXFRAME_DERIVED* _pFrame, const _matrix& _ParentMatrix);
	// �޽� �����̳ʰ� ������ ��� ���� ��ȯ ����� �޽� �����̳ʿ��� �����ϵ��� ������
	HRESULT SetUpCombinedTransformationMatrixPointer(D3DXFRAME_DERIVED* _pFrame);

public:
	// �������� �Լ�
	static CDynamicMesh* CreateProto(LPDIRECT3DDEVICE9 _pGrahpicDevice, const _tchar* _szMeshPath, const _tchar* _szMeshName, const _matrix* _pLocalTransMatrix = nullptr);
	// ���� ���� �Լ�
	virtual CComponent* CreateClone(void* _pArgument);

public:
	// Free
	virtual void Free();
};

END

#endif // !__DYNAMIC_MESH_H__
