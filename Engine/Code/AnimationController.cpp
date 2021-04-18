#include "..\Header\AnimationController.h"

// 생성자
CAnimationController::CAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController)
	: m_pAnimationController(_pAnimationController)
	, m_uiCurrentTrack(0)
	, m_uiNewTrack(0)
	, m_uiCurrentAnimationSet(0)
	, m_dTimeAcc(0)
	, m_dPeriod(0){
	SafeAddRef(m_pAnimationController);
}

// 복사 생성자
CAnimationController::CAnimationController(const CAnimationController & _AniamtionController)
	: m_pAnimationController(nullptr)
	, m_uiCurrentTrack(0)
	, m_uiNewTrack(0)
	, m_uiCurrentAnimationSet(0)
	, m_dTimeAcc(0)
	, m_dPeriod(0){
	if(nullptr != _AniamtionController.m_pAnimationController){
		// 애니메이션컨트롤러의 복제, 인자로 다 있는 값을 넘겨주면 된다.
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

// 인덱스에 해당하는 애니메이션을 설정함
HRESULT CAnimationController::SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed /*= 1.f*/){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	if(_uiIndex == m_uiCurrentAnimationSet)
		return NOERROR;

	LPD3DXANIMATIONSET pAnimationSet = nullptr;

	// 이전에 재생중인 애니메이션이 있다면 새 애니메이션은 1번 트랙에 재생되도록 한다.
	m_uiNewTrack = /*m_uiCurrentTrack == 0 ? 1 :*/ 0;

	// 애니메이션 컨트롤러에서 인덱스에 해당하는 애니메이션 셋을 가져옴
	m_pAnimationController->GetAnimationSet(_uiIndex, &pAnimationSet);
	// 가져온 애니메이션 셋을 트랙에 설정함
	m_pAnimationController->SetTrackAnimationSet(m_uiNewTrack, pAnimationSet);

	m_pAnimationController->SetTrackSpeed(m_uiNewTrack, _fAnimationSpeed);

	// 피리오드는 애니메이션이 재생되는 시간을 뜻한다.(값을 적당히 보정하여 저장함)
	m_dPeriod = pAnimationSet->GetPeriod() - 0.05;

	SafeRelease(pAnimationSet);

	//// 애니메이션에 이벤트가 있는경우 보간설정이 안되기 때문에 미리 설정을 해제함
	//m_pAnimationController->UnkeyAllTrackEvents(m_uiCurrentTrack);
	//m_pAnimationController->UnkeyAllTrackEvents(m_uiNewTrack);

	//// 이전 애니메이션이 종료되는 시간을 애니메이션셋이 변경된 지점으로부터 조금 더 늦게 끝나게 한다.
	//m_pAnimationController->KeyTrackEnable(m_uiCurrentTrack, FALSE, m_dTimeAcc + 0.05);
	//// 애니메이션의 속도를 설정함, 또 다른 트랙에 재생중인 애니메이션과의 속도를 보정 한다
	//m_pAnimationController->KeyTrackSpeed(m_uiCurrentTrack, 1.f, m_dTimeAcc, 0.01, D3DXTRANSITION_LINEAR);
	//// 애니메이션의 비중 설정, 다른 트랙에 재생중인 애니메이션과의 비중을 계산하여 보정한다.
	//m_pAnimationController->KeyTrackWeight(m_uiCurrentTrack, 0.1f, m_dTimeAcc, 0.01, D3DXTRANSITION_LINEAR);

	//// 현재 애니메이션을 현재 시간에 재생되도록 설정함
	//m_pAnimationController->SetTrackEnable(m_uiNewTrack, TRUE);
	//// 애니메이션의 속도를 설정함, 또 다른 트랙에 재생중인 애니메이션과의 속도를 보정 한다
	//m_pAnimationController->KeyTrackSpeed(m_uiNewTrack, 1.f, m_dTimeAcc, 0.05, D3DXTRANSITION_LINEAR);
	//// 애니메이션의 비중 설정, 다른 트랙에 재생중인 애니메이션과의 비중을 계산하여 보정한다.
	//m_pAnimationController->KeyTrackWeight(m_uiNewTrack, 0.9f, m_dTimeAcc, 0.05, D3DXTRANSITION_LINEAR);

	/* 현재 재생중인 애니메이션보다 다음에 재생될 애니메이션의 비중을 크게 해서
	두 트랙의 애니메이션이 동시에 재생 되는 경우 
	이전 애니메이션에서 바뀌는 애니메이션으로 서서히 바뀌는 모습으로 만든다. */

	// 재생되는 애니메이션의 트랙번호를 저장함
	m_uiCurrentTrack = m_uiNewTrack;

	// 지금까지 누적된 애니메이션 재생시간을 초기화함
	m_pAnimationController->ResetTime();
	m_dTimeAcc = 0.0;

	// 트랙의 재생위치를 설정함
	/* 애니메이션이 바뀌었으므로 처음부터 재생하도록 함 */
	m_pAnimationController->SetTrackPosition(m_uiCurrentTrack, 0.0);

	m_uiCurrentAnimationSet = _uiIndex;

	return NOERROR;
}

// 애니메이션 재생
HRESULT CAnimationController::PlayAnimationSet(_double _dDeltaTime){
	if(nullptr == m_pAnimationController)
		return E_FAIL;

	// 애니메이션을 재생시킴(내부적으로 재생한 정도를 누적함)
	m_pAnimationController->AdvanceTime(_dDeltaTime, nullptr);
	// 내부에서 누적한 재생값을 꺼내올 수 없으므로 따로 같이 누적시킴
	m_dTimeAcc += _dDeltaTime;

	return NOERROR;
}

// 현재 재생중인 애니메이션이 끝나는 지점을 지났는지 확인
_bool CAnimationController::IsFinished() const{
	// 트랙 정보를 가지는 구조체 변수
	D3DXTRACK_DESC		TrackDesc;
	// 해당 트랙의 정보를 얻어옴
	m_pAnimationController->GetTrackDesc(m_uiCurrentTrack, &TrackDesc);

	// 트랙의 재생 위치가 현재 재생하는 애니메이션의 피리오드를 넘어서면 애니메이션이 끝난 것으로 간주함
	if(m_dPeriod <= TrackDesc.Position){
		//TrackDesc.Position -= m_dPeriod;
		//m_pAnimationController->SetTrackDesc(m_uiCurrentTrack, &TrackDesc);
		return true;
	}

	return false;
}

// 원본 생성 함수
CAnimationController * CAnimationController::CreateAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController){
	CAnimationController* pInstance = new CAnimationController(_pAnimationController);
	if(FAILED(pInstance->InitAnimationControl())){
		SafeRelease(pInstance);
		MSG_BOX(L"CAnimationController Create Failure");
	}

	return pInstance;
}

// 복제 생성 함수
CAnimationController * CAnimationController::CloneAnimationController(){
	return new CAnimationController(*this);
}

// Free
void CAnimationController::Free(){
	SafeRelease(m_pAnimationController);
}
