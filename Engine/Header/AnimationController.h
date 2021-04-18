#ifndef __ANIMATION_CONTROLLER_H__
#define __ANIMATION_CONTROLLER_H__

#include "Base.h"

BEGIN(Engine)

class CAnimationController:
	public CBase{
private:
	// ������
	explicit CAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController);
	// ���� ������
	explicit CAnimationController(const CAnimationController& AniamtionController);
	// �Ҹ���
	virtual ~CAnimationController() = default;

private:
	// Inintialize
	HRESULT InitAnimationControl();
public:
	// �ε����� �ش��ϴ� �ִϸ��̼��� ������
	HRESULT SetUpAnimationSet(_uint _uiIndex, _float _fAnimationSpeed);
	// �ִϸ��̼� ���
	HRESULT PlayAnimationSet(_double _dDeltaTime);
	// ���� ������� �ִϸ��̼��� �������� Ȯ��
	_bool IsFinished() const;
	// ���� ������� Ʈ���� ���� ����ð��� �ʱ�ȭ��
	void ResetTime(){
		m_pAnimationController->ResetTime();
		m_pAnimationController->SetTrackPosition(m_uiCurrentTrack, 0.0);
	}
	// ���� ������� �ִϸ��̼� ��Ʈ�� �ε����� ������
	_uint GetCurrentAnaimationSet(){
		return m_uiCurrentAnimationSet;
	}

private:
	// �ִϸ��̼� ��Ʈ�ѷ� ����
	LPD3DXANIMATIONCONTROLLER m_pAnimationController = nullptr;
	// ���� ������� �ִϸ��̼� ��Ʈ
	_uint m_uiCurrentAnimationSet = 0;
	// ���� ������� Ʈ�� ��ȣ
	_uint m_uiCurrentTrack = 0;
	// ���� ����� Ʈ�� ��ȣ
	_uint m_uiNewTrack = 0;

	// �ִϸ��̼� ����ð� ����
	_double m_dTimeAcc = 0.0;
	// �ִϸ��̼��� ������ �ð�
	_double m_dPeriod = 0.0;
public:
	// ���� ���� �Լ�(static)
	static CAnimationController* CreateAnimationController(LPD3DXANIMATIONCONTROLLER _pAnimationController);
	// ���� ���� �Լ�
	CAnimationController* CloneAnimationController();

public:
	virtual void Free();
};

END


#endif // !__ANIMATION_CONTROL_H__
