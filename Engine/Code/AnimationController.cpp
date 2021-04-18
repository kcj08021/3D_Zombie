#include "..\Header\AnimationController.h"

// ������
CAnimationController::CAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController)
	: m_pAnimationController(_pAnimationController)
	, m_uiCurrentTrack(0)
	, m_uiNewTrack(0)
	, m_uiCurrentAnimationSet(0)
	, m_dTimeAcc(0)
	, m_dPeriod(0){
	SafeAddRef(m_pAnimationController);
}

// ���� ������
CAnimationController::CAnimationController(const CAnimationController & _AniamtionController)
	: m_pAnimationController(nullptr)
	, m_uiCurrentTrack(0)
	, m_uiNewTrack(0)
	, m_uiCurrentAnimationSet(0)
	, m_dTimeAcc(0)
	, m_dPeriod(0){
	if(nullptr != _AniamtionController.m_pAnimationController){
		// �ִϸ��̼���Ʈ�ѷ��� ����, ���ڷ� �� �ִ� ���� �Ѱ��ָ� �ȴ�.
		_AniamtionController.m_pAnimationController->CloneAnimationController(
			_AniamtionController.m_pAnimationController->GetMaxNumAnimationOutputs(),
			_AniamtionController.m_pAnimationController->GetMaxNumAnimationSets(),
			_AniamtionController.m_pAnimationController->GetMaxNumTracks(),
			_AniamtionController.m_pAnimationController->GetMaxNumEvents(),
			&m_pAnimationController
		);
	}
}

// Initialize
HRESULT CAnimationController::InitAnimationControl(){
	return NOERROR;
}

// �ε����� �ش��ϴ� �ִϸ��̼��� ������
HRESULT CAnimationController::SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed /*= 1.f*/){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	if(_uiIndex == m_uiCurrentAnimationSet)
		return NOERROR;

	LPD3DXANIMATIONSET pAnimationSet = nullptr;

	// ������ ������� �ִϸ��̼��� �ִٸ� �� �ִϸ��̼��� 1�� Ʈ���� ����ǵ��� �Ѵ�.
	m_uiNewTrack = /*m_uiCurrentTrack == 0 ? 1 :*/ 0;

	// �ִϸ��̼� ��Ʈ�ѷ����� �ε����� �ش��ϴ� �ִϸ��̼� ���� ������
	m_pAnimationController->GetAnimationSet(_uiIndex, &pAnimationSet);
	// ������ �ִϸ��̼� ���� Ʈ���� ������
	m_pAnimationController->SetTrackAnimationSet(m_uiNewTrack, pAnimationSet);

	m_pAnimationController->SetTrackSpeed(m_uiNewTrack, _fAnimationSpeed);

	// �Ǹ������ �ִϸ��̼��� ����Ǵ� �ð��� ���Ѵ�.(���� ������ �����Ͽ� ������)
	m_dPeriod = pAnimationSet->GetPeriod() - 0.05;

	SafeRelease(pAnimationSet);

	//// �ִϸ��̼ǿ� �̺�Ʈ�� �ִ°�� ���������� �ȵǱ� ������ �̸� ������ ������
	//m_pAnimationController->UnkeyAllTrackEvents(m_uiCurrentTrack);
	//m_pAnimationController->UnkeyAllTrackEvents(m_uiNewTrack);

	//// ���� �ִϸ��̼��� ����Ǵ� �ð��� �ִϸ��̼Ǽ��� ����� �������κ��� ���� �� �ʰ� ������ �Ѵ�.
	//m_pAnimationController->KeyTrackEnable(m_uiCurrentTrack, FALSE, m_dTimeAcc + 0.05);
	//// �ִϸ��̼��� �ӵ��� ������, �� �ٸ� Ʈ���� ������� �ִϸ��̼ǰ��� �ӵ��� ���� �Ѵ�
	//m_pAnimationController->KeyTrackSpeed(m_uiCurrentTrack, 1.f, m_dTimeAcc, 0.01, D3DXTRANSITION_LINEAR);
	//// �ִϸ��̼��� ���� ����, �ٸ� Ʈ���� ������� �ִϸ��̼ǰ��� ������ ����Ͽ� �����Ѵ�.
	//m_pAnimationController->KeyTrackWeight(m_uiCurrentTrack, 0.1f, m_dTimeAcc, 0.01, D3DXTRANSITION_LINEAR);

	//// ���� �ִϸ��̼��� ���� �ð��� ����ǵ��� ������
	//m_pAnimationController->SetTrackEnable(m_uiNewTrack, TRUE);
	//// �ִϸ��̼��� �ӵ��� ������, �� �ٸ� Ʈ���� ������� �ִϸ��̼ǰ��� �ӵ��� ���� �Ѵ�
	//m_pAnimationController->KeyTrackSpeed(m_uiNewTrack, 1.f, m_dTimeAcc, 0.05, D3DXTRANSITION_LINEAR);
	//// �ִϸ��̼��� ���� ����, �ٸ� Ʈ���� ������� �ִϸ��̼ǰ��� ������ ����Ͽ� �����Ѵ�.
	//m_pAnimationController->KeyTrackWeight(m_uiNewTrack, 0.9f, m_dTimeAcc, 0.05, D3DXTRANSITION_LINEAR);

	/* ���� ������� �ִϸ��̼Ǻ��� ������ ����� �ִϸ��̼��� ������ ũ�� �ؼ�
	�� Ʈ���� �ִϸ��̼��� ���ÿ� ��� �Ǵ� ��� 
	���� �ִϸ��̼ǿ��� �ٲ�� �ִϸ��̼����� ������ �ٲ�� ������� �����. */

	// ����Ǵ� �ִϸ��̼��� Ʈ����ȣ�� ������
	m_uiCurrentTrack = m_uiNewTrack;

	// ���ݱ��� ������ �ִϸ��̼� ����ð��� �ʱ�ȭ��
	m_pAnimationController->ResetTime();
	m_dTimeAcc = 0.0;

	// Ʈ���� �����ġ�� ������
	/* �ִϸ��̼��� �ٲ�����Ƿ� ó������ ����ϵ��� �� */
	m_pAnimationController->SetTrackPosition(m_uiCurrentTrack, 0.0);

	m_uiCurrentAnimationSet = _uiIndex;

	return NOERROR;
}

// �ִϸ��̼� ���
HRESULT CAnimationController::PlayAnimationSet(_double _dDeltaTime){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	// �ִϸ��̼��� �����Ŵ(���������� ����� ������ ������)
	m_pAnimationController->AdvanceTime(_dDeltaTime, nullptr);
	// ���ο��� ������ ������� ������ �� �����Ƿ� ���� ���� ������Ŵ
	m_dTimeAcc += _dDeltaTime;

	return NOERROR;
}

// ���� ������� �ִϸ��̼��� ������ ������ �������� Ȯ��
_bool CAnimationController::IsFinished() const{
	// Ʈ�� ������ ������ ����ü ����
	D3DXTRACK_DESC		TrackDesc;
	// �ش� Ʈ���� ������ ����
	m_pAnimationController->GetTrackDesc(m_uiCurrentTrack, &TrackDesc);

	// Ʈ���� ��� ��ġ�� ���� ����ϴ� �ִϸ��̼��� �Ǹ����带 �Ѿ�� �ִϸ��̼��� ���� ������ ������
	if(m_dPeriod <= TrackDesc.Position){
		//TrackDesc.Position -= m_dPeriod;
		//m_pAnimationController->SetTrackDesc(m_uiCurrentTrack, &TrackDesc);
		return true;
	}

	return false;
}

// ���� ���� �Լ�
CAnimationController * CAnimationController::CreateAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController){
	CAnimationController* pInstance = new CAnimationController(_pAnimationController);
	if(FAILED(pInstance->InitAnimationControl())){
		SafeRelease(pInstance);
		MSG_BOX(L"CAnimationController Create Failure");
	}

	return pInstance;
}

// ���� ���� �Լ�
CAnimationController * CAnimationController::CloneAnimationController(){
	return new CAnimationController(*this);
}

// Free
void CAnimationController::Free(){
	SafeRelease(m_pAnimationController);
}
