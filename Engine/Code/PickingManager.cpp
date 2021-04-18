#include "..\Header\PickingManager.h"


IMPLEMENT_SINGLETON(CPickingManager);

// 생성자
CPickingManager::CPickingManager(){
}

// 피킹 대상 추가
HRESULT CPickingManager::AddPickee(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// 중복 대상 검사
	PICKEE_LIST::const_iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() != find_iter)
		return E_FAIL;

	// 컨테이너에 추가
	m_PickeeList.push_back(*_pPickeeDesc);
	
	return NOERROR;
}


HRESULT CPickingManager::ChangePickeeDesc(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// 중복 대상 검사
	PICKEE_LIST::iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() == find_iter)
		return E_FAIL;

	// 컨테이너에 저장된 피킹 대상 정보 변경
	m_PickeeList.erase(find_iter);
	m_PickeeList.push_back(*_pPickeeDesc);

	return NOERROR;
}

// 피킹 대상 제거
HRESULT CPickingManager::RemovePickeeDesc(CPickee::PICKEE_DESC * _pPickeeDesc){
	if(nullptr == _pPickeeDesc)
		return E_FAIL;

	// 중복 대상 검사
	PICKEE_LIST::const_iterator find_iter = find_if(m_PickeeList.begin(), m_PickeeList.end(), [_pPickeeDesc](CPickee::PICKEE_DESC& _PickeeDesc)->bool{
		return _PickeeDesc.pGameObj == _pPickeeDesc->pGameObj;
	});

	if(m_PickeeList.end() == find_iter)
		return E_FAIL;

	// 컨테이너에 저장된 피킹 대상 정보 변경
	m_PickeeList.erase(find_iter);

	return NOERROR;
}

// 피킹 체크(방향 벡터와 위치 벡터)
_bool CPickingManager::CheckPicking(const _vec3 & _vDirection, const _vec3 & _vPostion, CPicker::PICK_RESULT_LIST& _listResult){
	return DoCheckingPick(_vDirection, _vPostion, _listResult);
}

// 실제로 피킹을 체크하는 함수(특정 대상 없이 컨테이너 전체 순회하며 검사)
_bool CPickingManager::DoCheckingPick(const _vec3 & _vRayDir, const _vec3 & _vRayPos, CPicker::PICK_RESULT_LIST& _listResult){
	// 피킹 대상 컨테이너 순회
	for(auto& PickeeDesc : m_PickeeList){
		_vec3 vRayDir, vRayPos;
		// 피킹 대상이 가진 월드행렬의 역행렬을 계산
		_matrix matWorldInv;
		D3DXMatrixInverse(&matWorldInv, nullptr ,PickeeDesc.pWolrdMatrix);

		// 레이의 좌표와 방향에 피킹 대상의 월드 행렬의 역행렬을 곱함(피킹대상의 로컬 스페이스로 변환)
		D3DXVec3TransformNormal(&vRayDir, &_vRayDir, &matWorldInv);
		D3DXVec3TransformCoord(&vRayPos, &_vRayPos, &matWorldInv);

		// 충돌 결과를 담을 구조체 변수
		CPicker::PickingResult TempResult = {};

		// 피킹 대상의 폴리곤 갯수만큼 반복하여 검사(폴리곤 갯수가 없으면 메쉬 검사)
		if(0 == PickeeDesc.uiPrimitiveCount){
			// 메쉬와 레이의 충돌 검사시 필요한 변수들
			BOOL bHit = FALSE;
			DWORD dwFaceIndex = 0;
			LPD3DXBUFFER pAllHitBuffer = nullptr;
			DWORD dwHitCount = 0;

			// 실제로 충돌이 되는지 검사
			D3DXIntersect(PickeeDesc.pMesh, &vRayPos, &vRayDir, &bHit, &dwFaceIndex, &TempResult.fU, &TempResult.fV, &TempResult.fDistance, &pAllHitBuffer, &dwHitCount);
			if(TRUE == bHit){
				// 충돌된 메시의 두깨 체크
				D3DXINTERSECTINFO* pInfo = reinterpret_cast<D3DXINTERSECTINFO*>(pAllHitBuffer->GetBufferPointer());
				if(dwHitCount > 1)
					TempResult.fThickness = pInfo[1].Dist - pInfo[0].Dist;
				else
					TempResult.fThickness = 0.f;

				// 충돌 된 객체의 주소를 결과구조체에 저장
				TempResult.pGameObject = PickeeDesc.pGameObj;
				// 충돌이 된 객체의 트랜스폼 컴포넌트를 결과 구조체에 저장
				TempResult.pWolrdMatrix = PickeeDesc.pWolrdMatrix;
				// 결과 리스트에 결과 구조체 보관
				_listResult.push_back(TempResult);
			}
		} else
			for(_uint i = 0; i < PickeeDesc.uiPrimitiveCount; ++i){
				// 폴리곤을 이루는 각 세 정점의 위치를 가지는 구조체 변수
				const POLYGON_POSITION& pPolygonPos = PickeeDesc.pPolygonPosition[i];

				// 실제로 충돌이 되는지 검사
				if(D3DXIntersectTri(&pPolygonPos._1, &pPolygonPos._2, &pPolygonPos._3, &vRayPos, &vRayDir, &TempResult.fU, &TempResult.fV, &TempResult.fDistance)){
					// 충돌 된 객체의 주소를 결과구조체에 저장
					TempResult.pGameObject = PickeeDesc.pGameObj;
					// 충돌이 된 객체의 트랜스폼 컴포넌트를 결과 구조체에 저장
					TempResult.pWolrdMatrix = PickeeDesc.pWolrdMatrix;
					// 결과 리스트에 결과 구조체 보관
					_listResult.push_back(TempResult);
				}
			}
	}

	////////////////////////////////////////////////////////////////////////
	// 충돌 결과가 2개 이상인 경우 정렬
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
