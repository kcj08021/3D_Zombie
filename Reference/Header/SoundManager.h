#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

// FMOD: 유니티, 언리얼과 같은 상용 엔진에도 두루 쓰이는 사운드 관련 엔진.
// 사운드에 관련된 인터페이스를 제공.

#include "Base.h"

BEGIN(Engine)

class CSoundManager 
	: public CBase{
	DECLARE_SINGLETON(CSoundManager)

public:
	enum CHANNEL_ID { BGM, PLAYER, MONSTER, EFFECT, MAX_CHANNEL };

private:
	explicit CSoundManager();
	virtual ~CSoundManager() = default;

public:
	void Initialize();
	void UpdateSound();
	void PlaySound(const wstring& wstrSoundKey, CHANNEL_ID eID);
	void PlayBGM(const wstring& wstrSoundKey);
	void StopSound(CHANNEL_ID eID);
	void StopAll();
	void SetVolume(CHANNEL_ID eID, float fVol);

private:
	void LoadSoundFile();

private:
	// FMOD_SOUND: 사운드 파일에 대한 정보를 갖는 구조체.
	map<wstring, FMOD_SOUND*>	m_MapSound;

	// FMOD_CHANNEL: 사운드를 재생하고 관리하는 역할. ex) 볼륨 조절 등.
	FMOD_CHANNEL*	m_pChannelArr[MAX_CHANNEL];

	// FMOD_SYSTEM: FMOD_SOUND과 FMOD_CHANNEL을 총괄 관리하는 객체.
	FMOD_SYSTEM*	m_pSystem;

public:
	// Free
	virtual void Free();
};

END

#endif // !__SOUND_MANAGER_H__


