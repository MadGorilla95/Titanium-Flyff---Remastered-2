#ifndef __MUSIC_H__
#define __MUSIC_H__
#pragma once

#ifdef __fmod
#include "../fmod/inc/fmod.hpp"
class SoundSystemClass
{
public:
    static FMOD::System* fSystem;
    static FMOD::Sound* soundMusic;
    static FMOD::Sound* ambienceMusic;
    static FMOD::Channel* channelMusic;
    static FMOD::Channel* channelAmbience;

    static std::map<unsigned int, std::string> musicList;

    static unsigned long curMusic;
    static void init();
    static void updateSystem();
    static void releaseSoundSystem();
    static void FmodErrorCheck(FMOD_RESULT&& result);
    static void FmodErrorCheck(FMOD_RESULT&& result, const int line);
    static void loadMusics();

    static void playMusic(int id = -1, const bool bLoop = true);


    // sounds
    static std::map<unsigned int, std::string> soundList;
    static std::vector<std::pair<FMOD::Channel*, FMOD::Sound*>> playingSounds;

    static void loadSound();
    static std::string getFileName(unsigned long id);

    static void playSound(unsigned int id);
#if _HAS_CXX17
    static void playSound(std::string_view fileName);
#else
    static void playSound(std::string&& fileName);
#endif

    static void playSound(unsigned int id, D3DXVECTOR3* soundPos);
    static void playSound(unsigned long id, FMOD_VECTOR&& soundPos);

    static void playSound(const std::string& file, D3DXVECTOR3* soundPos);
#if _HAS_CXX17
    static void playSound(std::string_view fileName, FMOD_VECTOR&& soundPos);
#else
    static void playSound(std::string&& fileName, FMOD_VECTOR&& soundPos);
#endif

    static void changeVolume(bool bMusic = true);

    static void setListener(const D3DXVECTOR3& vecPos, const float fAngle);
    static void setListener(D3DXVECTOR3&& vecPos, const float fAngle);
    static void playAmbience();
    static bool IsStopMusic();
    static void muteSounds(bool mute = true);
    static void muteMusic(bool mute = true);

    static bool isMuteSounds;
    static bool isMuteMusic;
    static bool isInit;
};

#else
class CMusicMng
{
public:
	struct MUSIC
	{
		TCHAR szMusicFileName[ 128 ];
	};
	CFixedArray< MUSIC > m_aMusic;

	BOOL LoadScript( LPCTSTR lpszFileName );
	LPTSTR GetFileName( DWORD dwId );
};
void ProcessFadeMusic();
BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount = 1 );
BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount = 1 );
BOOL PlayBGM( DWORD dwIdMusic );
void LockMusic();
void StopMusic();
void SetVolume( FLOAT fVolume );
FLOAT GetVolume();
BOOL IsPlayingMusic();
BOOL IsStopMusic();
void InitCustomSound( BOOL bEnable = TRUE );
void UnInitCustomSound();

#ifdef __MUTEBGSOUNDS
void ToggleMusicMute(bool mute);
#endif // __MUTEBGSOUNDS

extern CMusicMng g_MusicMng;

#endif
#endif
