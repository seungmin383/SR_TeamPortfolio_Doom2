#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CSoundManager :  public CBase
{
	DECLARE_SINGLETON(CSoundManager)

private:
	explicit CSoundManager();
	virtual ~CSoundManager();

public:
	void Init();
	void Release();

public:
	void PlaySoundW(const std::wstring& soundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const std::wstring& soundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile();

private:
	// 사운드 폴더의 최종 경로를 갖는 변수
	wstring m_strSoundFolderPath;

	// 사운드 리소스 정보를 갖는 객체 
	map<std::wstring, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

private:
	virtual void Free() override;
};

END