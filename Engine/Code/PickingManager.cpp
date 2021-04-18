#include "..\Header\PickingManager.h"


IMPLEMENT_SINGLETON(CPickingManager);

// ������
CPickingManager::CPickingManager(){
}

// ��ŷ ��� �߰�
HRESULT CPickingManager::AddPickee(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// �ߺ� ��� �˻�
	PICKEE_LIST::const_iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() != find_iter)
		return E_FAIL;

	// �����̳ʿ� �߰�
	m_PickeeList.push_back(*_pPickeeDesc);
	
	return NOERROR;
}


HRESULT CPickingManager::ChangePickeeDesc(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// �ߺ� ��� �˻�
	PICKEE_LIST::iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() == find_iter)
		return E_FAIL;

	// �����̳ʿ� ����� ��ŷ ��� ���� ����
	m_PickeeList.erase(find_iter);
	m_PickeeList.push_back(*_pPickeeDesc);

	return NOERROR;
}

// ��ŷ ��� ����
HRESULT CPickingManager::RemovePickeeDesc(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// �ߺ� ��� �˻�
	PICKEE_LIST::const_iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() == find_iter)
		return E_FAIL;

	// �����̳ʿ� ����� ��ŷ ��� ���� ����
	m_PickeeList.erase(find_iter);

	return NOERROR;
}

// ��ŷ üũ(���� ���Ϳ� ��ġ ����)
_bool CPickingManager::CheckPicking(const _vec3 & _vDirection, const _vec3 & _vPostion, CPicker::PICK_RESULT_LIST& _listResult){
	return DoCheckingPick(_vDirection, _vPostion, _listResult);
}

// ������ ��ŷ�� üũ�ϴ� �Լ�(Ư�� ��� ���� �����̳� ��ü ��ȸ�ϸ� �˻�)
_bool CPickingManager::DoCheckingPick(const _vec3 & _vRayDir, const _vec3 & _vRayPos, CPicker::PICK_RESULT_LIST& _listResult){
	// ��ŷ ��� �����̳� ��ȸ
	for(auto& PickeeDesc : m_PickeeList){
		_vec3 vRayDir, vRayPos;
		// ��ŷ ����� ���� ��������� ������� ���
		_matrix matWorldInv;
		D3DXMatrixInverse(&matWorldInv, nullptr ,PickeeDesc.pWolrdMatrix);

		// ������ ��ǥ�� ���⿡ ��ŷ ����� ���� ����� ������� ����(��ŷ����� ���� �����̽��� ��ȯ)
		D3DXVec3TransformNormal(&vRayDir, &_vRayDir, &matWorldInv);
		D3DXVec3TransformCoord(&vRayPos, &_vRayPos, &matWorldInv);

		// �浹 ����� ���� ����ü ����
		CPicker::PickingResult TempResult = {};

		// ��ŷ ����� ������ ������ŭ �ݺ��Ͽ� �˻�(������ ������ ������ �޽� �˻�)
		if(0 == PickeeDesc.uiPrimitiveCount){
			// �޽��� ������ �浹 �˻�� �ʿ��� ������
			BOOL bHit = FALSE;
			DWORD dwFaceIndex = 0;
			LPD3DXBUFFER pAllHitBuffer = nullptr;
			DWORD dwHitCount = 0;

			// ������ �浹�� �Ǵ��� �˻�
			D3DXIntersect(PickeeDesc.pMesh, &vRayPos, &vRayDir, &bHit, &dwFaceIndex, &TempResult.fU, &TempResult.fV, &TempResult.fDistance, &pAllHitBuffer, &dwHitCount);
			if(TRUE == bHit){
				// �浹�� �޽��� �α� üũ
				D3DXINTERSECTINFO* pInfo = reinterpret_cast<D3DXINTERSECTINFO*>(pAllHitBuffer->GetBufferPointer());
				if(dwHitCount > 1)
					TempResult.fThickness = pInfo[1].Dist - pInfo[0].Dist;
				else
					TempResult.fThickness = 0.f;

				// �浹 �� ��ü�� �ּҸ� �������ü�� ����
				TempResult.pGameObject = PickeeDesc.pGameObj;
				// �浹�� �� ��ü�� Ʈ������ ������Ʈ�� ��� ����ü�� ����
				TempResult.pWolrdMatrix = PickeeDesc.pWolrdMatrix;
				// ��� ����Ʈ�� ��� ����ü ����
				_listResult.push_back(TempResult);
			}
		} else
			for(_uint i = 0; i < PickeeDesc.uiPrimitiveCount; ++i){
				// �������� �̷�� �� �� ������ ��ġ�� ������ ����ü ����
				const POLYGON_POSITION& pPolygonPos = PickeeDesc.pPolygonPosition[i];

				// ������ �浹�� �Ǵ��� �˻�
				if(D3DXIntersectTri(&pPolygonPos._1, &pPolygonPos._2, &pPolygonPos._3, &vRayPos, &vRayDir, &TempResult.fU, &TempResult.fV, &TempResult.fDistance)){
					// �浹 �� ��ü�� �ּҸ� �������ü�� ����
					TempResult.pGameObject = PickeeDesc.pGameObj;
					// �浹�� �� ��ü�� Ʈ������ ������Ʈ�� ��� ����ü�� ����
					TempResult.pWolrdMatrix = PickeeDesc.pWolrdMatrix;
					// ��� ����Ʈ�� ��� ����ü ����
					_listResult.push_back(TempResult);
				}
			}
	}

	////////////////////////////////////////////////////////////////////////
	// �浹 ����� 2�� �̻��� ��� ����
	if(_listResult.size() >= 2){
		_listResult.sort([](CPicker::PickingResult& _src, CPicker::PickingResult& _Dest) ->_bool{
			return _src.fDistance < _Dest.fDistance;
		}
		);

	////////////////////////////////////////////////////////////////////////
	}else if(_listResult.size() == 0)
		return false;
	
	return true;
}

void CPickingManager::Free(){
	m_PickeeList.clear();
}
