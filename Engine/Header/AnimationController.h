#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "Base.h"

BEGIN(Engine)

class CAnimationController:
	public CBase{
private:
	// 생성자
	explicit CAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController);
	// 복사 생성자
	explicit CAnimationController(const CAnimationController& AniamtionController);
	// 소멸자
	virtual ~CAnimationController() = default;

private:
	// Inintialize
	HRESULT InitAnimationControl();
public:
	// 인덱스에 해당하는 애니메이션을 설정함
	HRESULT SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed);
	// 애니메이션 재생
	HRESULT PlayAnimationSet(_double _dDeltaTime);
	// 현재 재생중인 애니메이션이 끝났는지 확인
	_bool IsFinished() const;
	// 현재 재생중인 트랙의 누적 재생시간을 초기화함
	void ResetTime(){
		m_pAnimationController->ResetTime();
		m_pAnimationController->SetTrackPosition(m_uiCurrentTrack, 0.0);
	}
	// 현재 재생중인 애니메이션 세트의 인덱스를 가져옴
	_uint GetCurrentAnaimationSet(){
		return m_uiCurrentAnimationSet;
	}

private:
	// 애니메이션 컨트롤러 변수
	LPD3DXANIMATIONCONTROLLER m_pAnimationController = nullptr;
	// 현재 재생중인 애니메이션 세트
	_uint m_uiCurrentAnimationSet = 0;
	// 현재 재생중인 트랙 번호
	_uint m_uiCurrentTrack = 0;
	// 새로 재생될 트랙 번호
	_uint m_uiNewTrack = 0;

	// 애니메이션 재생시간 누적
	_double m_dTimeAcc = 0.0;
	// 애니메이션이 끝나는 시간
	_double m_dPeriod = 0.0;
public:
	// 원본 생성 함수(static)
	static CAnimationController* CreateAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController);
	// 복제 생성 함수
	CAnimationController* CloneAnimationController();

public:
	virtual void Free();
};

END


#endif // !__ANIMATION_CONTROL_H__
