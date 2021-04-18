#include "..\Header\Picker.h"
#include "..\Header\Management.h"
#include "..\Header\PickingManager.h"
#include "..\Header\Transform.h"


// 생성자
CPicker::CPicker(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pPickingManager(nullptr){
}

// 복사 생성자
CPicker::CPicker(const CPicker & _Picker)
	: CComponent(_Picker)
	, m_pPickingManager(_Picker.m_pPickingManager){
	SafeAddRef(_Picker.m_pPickingManager);
}

// Initialize with Prototype
HRESULT CPicker::InitComponent_Proto(){
	m_pPickingManager = CPickingManager::GetInstance();
	if(nullptr == m_pPickingManager)
		return E_FAIL;
	SafeAddRef(m_pPickingManager);

	return NOERROR;
}

// Initialize with Clone
HRESULT CPicker::InitComponent_Clone(void* _pArgument){
	return NOERROR;
}

_vec3 CPicker::GetFirstIntersectPoint(const _vec3 & _vRayDir, const _vec3& _vRayPos){
	_vec3 vRayDir, vRayPos, vResult;

	// 맨 앞의 원소(가장 가까운 충돌 객체)의 월드행렬의 역행렬 계산
	_matrix matWorldInv;
	D3DXMatrixInverse(&matWorldInv, nullptr, m_ListPickingResult.front().pWolrdMatrix);


	// 위의 역행렬로 마우스의 방향벡터와 위치벡터 변환(로컬 스페이스)
	D3DXVec3TransformNormal(&vRayDir, &_vRayDir, &matWorldInv);
	D3DXVec3TransformCoord(&vRayPos, &_vRayPos, &matWorldInv);

	// 충돌 지점 = 마우스의 좌표 + (마우스의 방향 벡터 * 충돌 지점까지의 거리)
	_vec3 vLocalLocation = vRayPos + (vRayDir * m_ListPickingResult.front().fDistance);

	// 계산한 충돌 지점과 월드 행렬간의 곰셈(월드 스페이스로의 변환)
	D3DXVec3TransformCoord(&vResult, &vLocalLocation, m_ListPickingResult.front().pWolrdMatrix);

	return vResult;
}

// 피킹 체크(마우스 좌표)
_bool CPicker::CheckPicking(const POINT& _tMousePt, _vec3 * _pOutLocation/* = nullptr*/){
	if(nullptr == m_pPickingManager)
		return false;

	// 마우스 좌표를 월드 스페이스로 변환
	_vec3 vRayPos, vRayDir;
	TransformMousePos(_tMousePt, vRayDir, vRayPos);

	m_ListPickingResult.clear();

	_bool bIsPicking = m_pPickingManager->CheckPicking(vRayDir, vRayPos, m_ListPickingResult);
	
	// 피킹된 지점중 가장 가까운 점의 위치 계산
	if(nullptr != _pOutLocation
		&& bIsPicking){
		*_pOutLocation = GetFirstIntersectPoint(vRayDir, vRayPos);
	}
	
	// 결과 반환
	return bIsPicking;
}

// 피킹 체크(방향벡터와 위치)
_bool CPicker::CheckPicking(const _vec3 & _vDirection, const _vec3& _vPostion, _vec3* _pOutLocation/* = nullptr*/){
	if(nullptr == m_pPickingManager)
		return false;

	m_ListPickingResult.clear();

	_bool bIsPicking = m_pPickingManager->CheckPicking(_vDirection, _vPostion, m_ListPickingResult);

	// 피킹된 지점중 가장 가까운 점의 위치 계산
	if(nullptr != _pOutLocation
		&& bIsPicking){
		*_pOutLocation = GetFirstIntersectPoint(_vDirection, _vPostion);
	}

	// 결과 반환
	return bIsPicking;
}

HRESULT CPicker::TransformMousePos(const POINT & _tInMousePos, _vec3 & _vOutRayDir, _vec3 & _vOutRayPos){
		CManagement* pManagment = CManagement::GetInstance();
		if(nullptr == pManagment)
			return E_FAIL;

		SafeAddRef(pManagment);
		/////////////////////////////////////////////////////////////////////////
		/*뷰포트에서 투영 스페이스로의 변환*/
		// 뷰포트 변수
		D3DVIEWPORT9 ViewPort;
		// 현재 사용되는 뷰포트의 정보를 가져옴
		pManagment->GetGraphicDevice()->GetViewport(&ViewPort);

		// 마우스의 위치벡터는 투영스페이스에서 항상 원점
		_vOutRayPos = { 0.f,0.f,0.f };

		// 투영스페이스에서 마우스의 방향 벡터(-1,1에서 1,-1의 범위에 들어가는 좌표)
		_vOutRayDir.x = _tInMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
		_vOutRayDir.y = _tInMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
		_vOutRayDir.z = 0.f;

		/////////////////////////////////////////////////////////////////////////
		/*투영 스페이스에서 뷰 스페이스로의 변환*/
		_matrix matProj = {};

		// 투영 행렬 얻어옴
		if(FAILED(pManagment->GetTransform(D3DTS_PROJECTION, &matProj))){
			SafeRelease(pManagment);
			return E_FAIL;
		}
		// 투영 행렬의 역행렬 계산
		D3DXMatrixInverse(&matProj, nullptr, &matProj);
		// 투영행렬의 역행렬을 마우스의 방향벡터에 곱함
		D3DXVec3TransformCoord(&_vOutRayDir, &_vOutRayDir, &matProj);

		// D3DXVec3TransformCoord() 함수는 _vec3와 행렬과 곱하는 함수
		// 이때 w의 자리에 1로 채워서 계산을 진행한다.
		// 또 행렬과 곱셈 후 w의 값으로 x, y, z를 나누는 작업도 진행한다.
		// 투영 행렬의 역행력에는 1/(뷰스페이스 상의 z)이 들어있어 곱셈 후 w의 자리엔 1/(뷰스페이스 상의 z)가 들어가게 된다.
		// 따라서 D3DXVec3TransformCoord()를 이용하여 투영 행렬의 역행렬을 곱하면 뷰스페이스 상의 좌표로 복구가 된다.

		/////////////////////////////////////////////////////////////////////////
		/* 뷰 스페이스에서 월드 스페이스로의 변환*/
		_matrix matCameraWorld = {};
		// 카메라의 월드 행렬을 얻어옴(사실 뷰 변환 행렬의 역행렬로 구할 수 있음)
		if(FAILED(pManagment->GetCameraWorldMatrix(&matCameraWorld))){
			SafeRelease(pManagment);
			return E_FAIL;
		}

		// 마우스의 위치 벡터와 카메라의 월드행렬을 곱함(D3DXVec3TransformCoord())
		D3DXVec3TransformCoord(&_vOutRayPos, &_vOutRayPos, &matCameraWorld);
		// 마우스의 방향 벡터와 카메라의 월드행렬을 곱함(D3DXVec3TransformNormal())
		D3DXVec3TransformNormal(&_vOutRayDir, &_vOutRayDir, &matCameraWorld);

		/////////////////////////////////////////////////////////////////////////

		SafeRelease(pManagment);

		return NOERROR;
}

// 원본 생성 함수(static)
CPicker * CPicker::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CPicker* pInstance = new CPicker(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CPicker Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CComponent * CPicker::CreateClone(void* _pArgument){
	CComponent* pInstance = new CPicker(*this);
	if(pInstance->InitComponent_Clone(_pArgument)){
		SafeRelease(pInstance);
		MSG_BOX(L"CPicker Clone Failure");
	}

	return pInstance;
}

// Free
void CPicker::Free(){
	SafeRelease(m_pPickingManager);

	CComponent::Free();
}
