#include "..\Header\Picker.h"
#include "..\Header\Management.h"
#include "..\Header\PickingManager.h"
#include "..\Header\Transform.h"


// ������
CPicker::CPicker(LPDIRECT3DDEVICE9 _pGraphicDevice)
	: CComponent(_pGraphicDevice)
	, m_pPickingManager(nullptr){
}

// ���� ������
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

	// �� ���� ����(���� ����� �浹 ��ü)�� ��������� ����� ���
	_matrix matWorldInv;
	D3DXMatrixInverse(&matWorldInv, nullptr, m_ListPickingResult.front().pWolrdMatrix);


	// ���� ����ķ� ���콺�� ���⺤�Ϳ� ��ġ���� ��ȯ(���� �����̽�)
	D3DXVec3TransformNormal(&vRayDir, &_vRayDir, &matWorldInv);
	D3DXVec3TransformCoord(&vRayPos, &_vRayPos, &matWorldInv);

	// �浹 ���� = ���콺�� ��ǥ + (���콺�� ���� ���� * �浹 ���������� �Ÿ�)
	_vec3 vLocalLocation = vRayPos + (vRayDir * m_ListPickingResult.front().fDistance);

	// ����� �浹 ������ ���� ��İ��� ����(���� �����̽����� ��ȯ)
	D3DXVec3TransformCoord(&vResult, &vLocalLocation, m_ListPickingResult.front().pWolrdMatrix);

	return vResult;
}

// ��ŷ üũ(���콺 ��ǥ)
_bool CPicker::CheckPicking(const POINT& _tMousePt, _vec3 * _pOutLocation/* = nullptr*/){
	if(nullptr == m_pPickingManager)
		return false;

	// ���콺 ��ǥ�� ���� �����̽��� ��ȯ
	_vec3 vRayPos, vRayDir;
	TransformMousePos(_tMousePt, vRayDir, vRayPos);

	m_ListPickingResult.clear();

	_bool bIsPicking = m_pPickingManager->CheckPicking(vRayDir, vRayPos, m_ListPickingResult);
	
	// ��ŷ�� ������ ���� ����� ���� ��ġ ���
	if(nullptr != _pOutLocation
		&& bIsPicking){
		*_pOutLocation = GetFirstIntersectPoint(vRayDir, vRayPos);
	}
	
	// ��� ��ȯ
	return bIsPicking;
}

// ��ŷ üũ(���⺤�Ϳ� ��ġ)
_bool CPicker::CheckPicking(const _vec3 & _vDirection, const _vec3& _vPostion, _vec3* _pOutLocation/* = nullptr*/){
	if(nullptr == m_pPickingManager)
		return false;

	m_ListPickingResult.clear();

	_bool bIsPicking = m_pPickingManager->CheckPicking(_vDirection, _vPostion, m_ListPickingResult);

	// ��ŷ�� ������ ���� ����� ���� ��ġ ���
	if(nullptr != _pOutLocation
		&& bIsPicking){
		*_pOutLocation = GetFirstIntersectPoint(_vDirection, _vPostion);
	}

	// ��� ��ȯ
	return bIsPicking;
}

HRESULT CPicker::TransformMousePos(const POINT & _tInMousePos, _vec3 & _vOutRayDir, _vec3 & _vOutRayPos){
		CManagement* pManagment = CManagement::GetInstance();
		if(nullptr == pManagment)
			return E_FAIL;

		SafeAddRef(pManagment);
		/////////////////////////////////////////////////////////////////////////
		/*����Ʈ���� ���� �����̽����� ��ȯ*/
		// ����Ʈ ����
		D3DVIEWPORT9 ViewPort;
		// ���� ���Ǵ� ����Ʈ�� ������ ������
		pManagment->GetGraphicDevice()->GetViewport(&ViewPort);

		// ���콺�� ��ġ���ʹ� ���������̽����� �׻� ����
		_vOutRayPos = { 0.f,0.f,0.f };

		// ���������̽����� ���콺�� ���� ����(-1,1���� 1,-1�� ������ ���� ��ǥ)
		_vOutRayDir.x = _tInMousePos.x / (ViewPort.Width * 0.5f) - 1.f;
		_vOutRayDir.y = _tInMousePos.y / -(ViewPort.Height * 0.5f) + 1.f;
		_vOutRayDir.z = 0.f;

		/////////////////////////////////////////////////////////////////////////
		/*���� �����̽����� �� �����̽����� ��ȯ*/
		_matrix matProj = {};

		// ���� ��� ����
		if(FAILED(pManagment->GetTransform(D3DTS_PROJECTION, &matProj))){
			SafeRelease(pManagment);
			return E_FAIL;
		}
		// ���� ����� ����� ���
		D3DXMatrixInverse(&matProj, nullptr, &matProj);
		// ��������� ������� ���콺�� ���⺤�Ϳ� ����
		D3DXVec3TransformCoord(&_vOutRayDir, &_vOutRayDir, &matProj);

		// D3DXVec3TransformCoord() �Լ��� _vec3�� ��İ� ���ϴ� �Լ�
		// �̶� w�� �ڸ��� 1�� ä���� ����� �����Ѵ�.
		// �� ��İ� ���� �� w�� ������ x, y, z�� ������ �۾��� �����Ѵ�.
		// ���� ����� ����¿��� 1/(�佺���̽� ���� z)�� ����־� ���� �� w�� �ڸ��� 1/(�佺���̽� ���� z)�� ���� �ȴ�.
		// ���� D3DXVec3TransformCoord()�� �̿��Ͽ� ���� ����� ������� ���ϸ� �佺���̽� ���� ��ǥ�� ������ �ȴ�.

		/////////////////////////////////////////////////////////////////////////
		/* �� �����̽����� ���� �����̽����� ��ȯ*/
		_matrix matCameraWorld = {};
		// ī�޶��� ���� ����� ����(��� �� ��ȯ ����� ����ķ� ���� �� ����)
		if(FAILED(pManagment->GetCameraWorldMatrix(&matCameraWorld))){
			SafeRelease(pManagment);
			return E_FAIL;
		}

		// ���콺�� ��ġ ���Ϳ� ī�޶��� ��������� ����(D3DXVec3TransformCoord())
		D3DXVec3TransformCoord(&_vOutRayPos, &_vOutRayPos, &matCameraWorld);
		// ���콺�� ���� ���Ϳ� ī�޶��� ��������� ����(D3DXVec3TransformNormal())
		D3DXVec3TransformNormal(&_vOutRayDir, &_vOutRayDir, &matCameraWorld);

		/////////////////////////////////////////////////////////////////////////

		SafeRelease(pManagment);

		return NOERROR;
}

// ���� ���� �Լ�(static)
CPicker * CPicker::CreateProto(LPDIRECT3DDEVICE9 _pGraphicDevice){
	CPicker* pInstance = new CPicker(_pGraphicDevice);
	if(FAILED(pInstance->InitComponent_Proto())){
		SafeRelease(pInstance);
		MSG_BOX(L"CPicker Create Failure");
	}

	return pInstance;
}

// ���� ���� �Լ�
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
