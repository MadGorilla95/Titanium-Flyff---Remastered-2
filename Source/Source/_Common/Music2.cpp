
// author : gmpbigsun
// date : 2009_11_02
// music player for ogg and mp3 ( use miles lib )

#include "StdAfx.h"
#include <stdexcept>


#ifdef __fmod
#include "Music.h"
#include "../fmod/inc/fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")


std::map<unsigned int, std::string> SoundSystemClass::musicList;
std::map<unsigned int, std::string> SoundSystemClass::soundList;
std::vector<std::pair<FMOD::Channel*, FMOD::Sound*>> SoundSystemClass::playingSounds;
FMOD::System* SoundSystemClass::fSystem;
FMOD::Sound* SoundSystemClass::soundMusic;
FMOD::Channel* SoundSystemClass::channelMusic;
unsigned long SoundSystemClass::curMusic;
bool SoundSystemClass::isMuteSounds;
bool SoundSystemClass::isMuteMusic;


void SoundSystemClass::init()
{
    FmodErrorCheck(FMOD::System_Create(&fSystem), __LINE__);

    int numDrivers = 0;
    FmodErrorCheck(fSystem->getNumDrivers(&numDrivers), __LINE__);

    if (numDrivers == 0)
        FmodErrorCheck(fSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND), __LINE__);
    else
    {
        fSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        FMOD_SPEAKERMODE fmodSpeaker;
        int systemRate = 0;
        FmodErrorCheck(fSystem->getDriverInfo(0, nullptr, 0, nullptr, &systemRate, &fmodSpeaker, nullptr), __LINE__);
        FmodErrorCheck(fSystem->setSoftwareFormat(systemRate, fmodSpeaker, 0), __LINE__);
    }
    FmodErrorCheck(fSystem->init(4093, FMOD_INIT_NORMAL, nullptr), __LINE__);

    changeVolume();
    changeVolume(false);
}

void SoundSystemClass::loadMusics()
{
    CScript s;
    if (!s.Load(MakePath(DIR_MUSIC, "default.bgm")))
    {
        Error("[Critical] Failed to load Default.bgm");
        return;
    }

    unsigned int id = s.GetNumber();
    while (s.tok != FINISHED)
    {
        s.GetToken();
        musicList.insert(std::pair<unsigned int, std::string>(id, s.token));
        id = s.GetNumber();
    }
}

void SoundSystemClass::changeVolume(const bool bMusic)
{
    if (!bMusic)
    {
        for (auto& v : playingSounds)
        {
            FmodErrorCheck(v.first->setPaused(true), __LINE__);
            FmodErrorCheck(v.first->setVolume(g_Option.m_fEffectVolume), __LINE__);
            FmodErrorCheck(v.first->setPaused(false), __LINE__);
        }
    }
    else if (channelMusic)
        FmodErrorCheck(channelMusic->setVolume(g_Option.m_fBGMVolume), __LINE__);
}

void SoundSystemClass::releaseSoundSystem()
{
    FmodErrorCheck(channelMusic->stop(), __LINE__);
    FmodErrorCheck(soundMusic->release(), __LINE__);
    FmodErrorCheck(fSystem->close(), __LINE__);
    FmodErrorCheck(fSystem->release(), __LINE__);
}

void SoundSystemClass::FmodErrorCheck(FMOD_RESULT&& result)
{
    if (result != FMOD_OK) 
    {
#ifdef __fmodDebug
        Error("[FMod] %d", result); 
#endif
    } 
}

void SoundSystemClass::FmodErrorCheck(FMOD_RESULT&& result, const int line)
{
    if (result != FMOD_OK)
    {
#ifdef __fmodDebug
        Error("[FMod] %d %d", result, line);
#endif
    }
}

void SoundSystemClass::playMusic(const int id, const bool bLoop)
{
    if (id <= 0 || static_cast<unsigned long>(id) == curMusic || id >= musicList.size())
        return;

    const std::string filename = musicList[id];
    if (filename.empty())
    {
        return;
    }


    curMusic = id;
    if (channelMusic && soundMusic)
    {
        FmodErrorCheck(channelMusic->stop(), __LINE__);
        FmodErrorCheck(soundMusic->release(), __LINE__);
    }

    const unsigned long paramaters = (bLoop) ? FMOD_LOOP_NORMAL | FMOD_CREATESTREAM : FMOD_DEFAULT | FMOD_CREATESTREAM;
    FmodErrorCheck(fSystem->createStream(filename.c_str(), paramaters, nullptr, &soundMusic), __LINE__);
    FmodErrorCheck(fSystem->playSound(soundMusic, nullptr, true, &channelMusic), __LINE__);

    if (isMuteMusic)
        FmodErrorCheck(channelMusic->setVolume(0.0f), __LINE__);
    else
        FmodErrorCheck(channelMusic->setVolume(g_Option.m_fBGMVolume), __LINE__);
    FmodErrorCheck(channelMusic->setPaused(false), __LINE__);
}

bool SoundSystemClass::IsStopMusic()
{
    bool isplay = false;
    FmodErrorCheck(channelMusic->isPlaying(&isplay), __LINE__);
    if (isplay)
        return false;

    return true;
}


void SoundSystemClass::setListener(const D3DXVECTOR3& vecPos, const float fAngle)
{
    const float fTheta = D3DXToRadian(fAngle);
    FMOD_VECTOR m_vOrientFront{ sinf(fTheta), 0.0f, cosf(fTheta) };

    if (fTheta > 3.14f || fTheta < 3.14f)
    {
        m_vOrientFront.x = -m_vOrientFront.x;
        m_vOrientFront.z = -m_vOrientFront.z;
    }

    FMOD_VECTOR m_vOrientTop{ 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR listenPos{ vecPos.x, vecPos.y, vecPos.z };
    FmodErrorCheck(fSystem->set3DListenerAttributes(0, &listenPos, nullptr, &m_vOrientFront, &m_vOrientTop), __LINE__);
}

void SoundSystemClass::setListener(D3DXVECTOR3&& vecPos, const float fAngle)
{
    const float fTheta = D3DXToRadian(fAngle);
    FMOD_VECTOR m_vOrientFront{ sinf(fTheta), 0.0f, cosf(fTheta) };

    if (fTheta > 3.14f || fTheta < 3.14f)
    {
        m_vOrientFront.x = -m_vOrientFront.x;
        m_vOrientFront.z = -m_vOrientFront.z;
    }
    FMOD_VECTOR m_vOrientTop{ 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR listenPos{ vecPos.x, vecPos.y, vecPos.z };
    FmodErrorCheck(fSystem->set3DListenerAttributes(0, &listenPos, nullptr, &m_vOrientFront, &m_vOrientTop), __LINE__);
}

void SoundSystemClass::loadSound()
{
    CScript s;
    if (s.Load(MakePath(DIR_CLIENT, _T("sound.inc"))) == FALSE)
    {
        Error("[Critical] Failure to load sound.inc");
        return;
    }

    unsigned int id = s.GetNumber();
    while (s.tok != FINISHED)
    {
        s.GetToken();
        soundList.insert(std::pair<unsigned int, std::string>(id, s.token));
        id = s.GetNumber();
    }
}

void SoundSystemClass::playSound(const unsigned int id, D3DXVECTOR3* soundPos)
{
    if (id == static_cast<unsigned int>(-1))
        return;

    std::string filename;
    try { filename = soundList.at(id); }
    catch (...) { return; }
    playSound(filename, soundPos);
}

std::string SoundSystemClass::getFileName(const unsigned long id)
{
    if (id == static_cast<unsigned int>(-1))
        return "";

    try {
        return soundList.at(id);
    }
    catch (...) {
        return "";
    }
}



void SoundSystemClass::playSound(const unsigned long id, FMOD_VECTOR&& soundPos)
{
    std::string fileName = getFileName(id);
    if (fileName.empty())
        return;
#if _HAS_CXX17
    playSound(fileName, std::move(soundPos));  // NOLINT(performance-move-const-arg)
#else
    playSound(std::move(fileName), std::move(soundPos));  // NOLINT(performance-move-const-arg)
#endif
}


void SoundSystemClass::playSound(const unsigned int id)
{
    std::string fileName = getFileName(id);
    if (fileName.empty())
        return;
#if _HAS_CXX17
    playSound(fileName);
#else
    playSound(std::move(fileName));
#endif
}

#if _HAS_CXX17
void SoundSystemClass::playSound(const std::string_view fileName)
#else
void SoundSystemClass::playSound(std::string&& fileName)
#endif
{
    FMOD::Sound* sound = nullptr;
    FmodErrorCheck(fSystem->createSound(MakePath(DIR_SOUND, fileName.data()), FMOD_DEFAULT | FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &sound), __LINE__);
    FmodErrorCheck(sound->set3DMinMaxDistance(0, 60), __LINE__);

    FMOD::Channel* soundChannel;
    FmodErrorCheck(fSystem->playSound(sound, nullptr, true, &soundChannel), __LINE__);
    playingSounds.emplace_back(soundChannel, sound);

    FMOD_VECTOR soundPos{};
    FmodErrorCheck(fSystem->get3DListenerAttributes(0, &soundPos, nullptr, nullptr, nullptr), __LINE__);


    FmodErrorCheck(soundChannel->set3DAttributes(&soundPos, nullptr), __LINE__);
    if (isMuteSounds)
        FmodErrorCheck(soundChannel->setVolume(0.0f), __LINE__);
    else
        FmodErrorCheck(soundChannel->setVolume(g_Option.m_fEffectVolume), __LINE__);

    FmodErrorCheck(soundChannel->setPaused(false));
}

#if _HAS_CXX17
void SoundSystemClass::playSound(const std::string_view fileName, FMOD_VECTOR&& soundPos)
#else
void SoundSystemClass::playSound(std::string&& fileName, FMOD_VECTOR&& soundPos)
#endif
{
    FMOD::Sound* sound = nullptr;
    FmodErrorCheck(fSystem->createSound(MakePath(DIR_SOUND, fileName.data()), FMOD_DEFAULT | FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &sound), __LINE__);
    FmodErrorCheck(sound->set3DMinMaxDistance(0, 60), __LINE__);

    FMOD::Channel* soundChannel;
    FmodErrorCheck(fSystem->playSound(sound, nullptr, true, &soundChannel), __LINE__);
    playingSounds.emplace_back(soundChannel, sound);

    FmodErrorCheck(soundChannel->set3DAttributes(&soundPos, nullptr), __LINE__);
    if (isMuteSounds)
        FmodErrorCheck(soundChannel->setVolume(0.0f), __LINE__);
    else
        FmodErrorCheck(soundChannel->setVolume(g_Option.m_fEffectVolume), __LINE__);

    FmodErrorCheck(soundChannel->setPaused(false), __LINE__);
}

void SoundSystemClass::playSound(const std::string& file, D3DXVECTOR3* soundPos)
{
    FMOD::Sound* sound = nullptr;
    FmodErrorCheck(fSystem->createSound(MakePath(DIR_SOUND, file.c_str()), FMOD_DEFAULT | FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &sound), __LINE__);
    FmodErrorCheck(sound->set3DMinMaxDistance(0, 60), __LINE__);

    FMOD::Channel* soundChannel;
    FmodErrorCheck(fSystem->playSound(sound, nullptr, true, &soundChannel), __LINE__);
    playingSounds.emplace_back(soundChannel, sound);

#ifdef __AEGON_FIXES
    FMOD_VECTOR vNewSound; 
    if (!soundPos) 
    {
        if (g_pPlayer)
        {
            D3DXVECTOR3 vPos = g_pPlayer->GetPos();
            vNewSound = FMOD_VECTOR{ vPos.x, vPos.y, vPos.z };
        }
        else
            vNewSound = FMOD_VECTOR{ 0, 0, 0 };
    }
    else
        vNewSound = FMOD_VECTOR{ soundPos->x, soundPos->y, soundPos->z };
#else
    FMOD_VECTOR vNewSound{ soundPos->x, soundPos->y, soundPos->z };
#endif

   
    FmodErrorCheck(soundChannel->set3DAttributes(&vNewSound, nullptr), __LINE__);
    if (isMuteSounds)
        FmodErrorCheck(soundChannel->setVolume(0.0f), __LINE__);
    else
        FmodErrorCheck(soundChannel->setVolume(g_Option.m_fEffectVolume), __LINE__);
    FmodErrorCheck(soundChannel->setPaused(false), __LINE__);
}

void SoundSystemClass::updateSystem()
{
    bool bplay = false;
    for (auto it = playingSounds.begin(); it != playingSounds.end();)
    {
        if (it->first && it->second)
        {
            const FMOD_RESULT res = it->first->isPlaying(&bplay);
            switch (res)
            {
            case FMOD_OK:
                if (!bplay)
                {
                    FmodErrorCheck(it->second->release(), __LINE__);
                    it = playingSounds.erase(it);
                }
                else
                    ++it;
                break;
            case FMOD_ERR_CHANNEL_STOLEN:
                it = playingSounds.erase(it);
                break;
            default: //invalid handle / all other errors
                FmodErrorCheck(it->second->release(), __LINE__);
                it = playingSounds.erase(it);
                break;
            }

        }
        else
        {
            if (it->second)
                FmodErrorCheck(it->second->release(), __LINE__);
            it = playingSounds.erase(it);
        }
    }
    FmodErrorCheck(fSystem->update(), __LINE__);
}

void SoundSystemClass::muteSounds(const bool mute)
{
    if (mute && !isMuteSounds)
    {
        isMuteSounds = true;
        for (auto& v : playingSounds)
        {
            FmodErrorCheck(v.first->setPaused(true));
            FmodErrorCheck(v.first->setVolume(0.0f));
            FmodErrorCheck(v.first->setPaused(false));
        }
    }
    else if (isMuteSounds)
    {
        isMuteSounds = false;
        for (auto& v : playingSounds)
        {
            FmodErrorCheck(v.first->setPaused(true));
            FmodErrorCheck(v.first->setVolume(g_Option.m_fEffectVolume));
            FmodErrorCheck(v.first->setPaused(false));
        }
    }
}

void SoundSystemClass::muteMusic(const bool mute)
{
    if (mute && !isMuteMusic)
    {
        isMuteMusic = true;
        FmodErrorCheck(channelMusic->setVolume(0.0f), __LINE__);
    }
    else if (isMuteMusic)
    {
        isMuteMusic = false;
        FmodErrorCheck(channelMusic->setVolume(g_Option.m_fBGMVolume), __LINE__);
    }
}

#else
#if __VER >= 15 // __MUSIC2

#include "Music.h"
#include "mss.h"

CMusicMng g_MusicMng;

static BOOL g_bEnableMusic = TRUE;
//static BOOL g_bLockMusic = FALSE;
static BOOL g_bFadeOutMusic = FALSE;
static TCHAR g_filename[128];
HSTREAM  g_Stream = NULL;
HDIGDRIVER  g_Dig = NULL;
TCHAR g_szFileName[64];

static float g_fMusicVolume = 1.0f;
static float g_fCurrVolume = 1.0f;


BOOL PlayMusic_( LPCTSTR lpszFileName, int nLoopCount )
{
	if( !g_Dig )
		return FALSE;

	if( g_bEnableMusic == FALSE )		// 사운드 카드가 없거나 껐을때
		return FALSE;

	if( _tcscmp( lpszFileName, g_filename ) == 0 && IsPlayingMusic() )
		return FALSE;

	// 아직 음악이 연주중이라면, 볼륨을 서서히 낮춰줄 세팅을 한다.
// 	if( IsPlayingMusic() )
// 	{
// 		_tcscpy( g_szFileName, lpszFileName );
// 		g_bFadeOutMusic = TRUE;
// 		g_fCurrVolume = g_fMusicVolume;
// 
// 		return TRUE;
// 	}

	// 제거 !
	if( g_Stream )
		AIL_close_stream( g_Stream );

	strcpy( g_filename, lpszFileName );
	
	// 새로운 스트림으로!
	g_Stream = AIL_open_stream( g_Dig, g_filename, 0 );

	if (!g_Stream)
	{
		Error("oh my god... AIL_open_stream error File: %s", g_filename);
		return FALSE;
	}
	
	if( nLoopCount < 0 )
		nLoopCount = 0;

	AIL_set_stream_loop_count( g_Stream, nLoopCount );
	AIL_start_stream( g_Stream );
	AIL_service_stream( g_Stream, 0 );
	
	// 볼륨은 원래대로
	SetVolume( g_fMusicVolume );

	return TRUE;
}


void ProcessFadeMusic()
{
	/*if( g_bFadeOutMusic )
	{
		g_fCurrVolume -= 0.01f;
		if( g_Dig )
			AIL_set_digital_master_volume_level( g_Dig, g_fCurrVolume );
		if( g_fMusicVolume < 0.0f )
		{
		    StopMusic();
			g_bFadeOutMusic = FALSE;
			g_bLockMusic = FALSE;
			if( PlayMusic_( g_szFileName, 0 ) )
				g_bLockMusic = TRUE;

		}
	}*/
}

BOOL PlayMusic( LPCTSTR lpszFileName, int nLoopCount )
{
	return PlayMusic_( lpszFileName, nLoopCount );
}

BOOL PlayMusic( DWORD dwIdMusic, int nLoopCount )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
	{	
		if( PlayMusic( lpFileName, nLoopCount ) )
			return TRUE;
	}
	return FALSE;
}

BOOL PlayBGM( DWORD dwIdMusic )
{
	if( dwIdMusic == NULL_ID || dwIdMusic == 0 ) //BGM_NONE )
		return FALSE;
	LPTSTR lpFileName = g_MusicMng.GetFileName( dwIdMusic );
	if( lpFileName[ 0 ] )
		return PlayMusic( lpFileName, 1 );
	
	return FALSE;
}

void LockMusic()
{
	//g_bLockMusic = TRUE;
}

void StopMusic()
{
	if( !g_Stream )
		return;

	AIL_pause_stream( g_Stream, 1 );		//1 pause, 0 palyback
}

void SetVolume( FLOAT fVolume )
{
	g_fMusicVolume = fVolume;	

	AIL_set_digital_master_volume_level( g_Dig, g_fMusicVolume );
}

FLOAT GetVolume()
{
	return g_fMusicVolume;
}

BOOL IsPlayingMusic()
{
///	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return FALSE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING == status );
}

BOOL IsStopMusic()
{
//	HSAMPLE hs = AIL_stream_sample_handle( g_Stream );
	if( !g_Stream )
		return TRUE;

	U32 status = AIL_stream_status( g_Stream );
	return ( SMP_PLAYING != status );

}

void InitCustomSound( BOOL bEnable )
{
	AIL_startup();

	g_bEnableMusic = bEnable;

	g_Dig = AIL_open_digital_driver( 44100, 16, MSS_MC_USE_SYSTEM_CONFIG, 0 );
	if( !g_Dig )
	{
		MessageBox(0,AIL_last_error(),"Music initialize error",MB_OK);		
		Error( "AIL_open_digital_driver error, Music init error" );
	}

}

void UnInitCustomSound()
{
	AIL_close_stream( g_Stream );
	AIL_close_digital_driver( g_Dig );
	AIL_shutdown();
}



BOOL CMusicMng::LoadScript( LPCTSTR lpszFileName )
{
	CScript script;
	if( script.Load( lpszFileName ) == FALSE )
		return FALSE;
	CString string;
	MUSIC music;
	int id = script.GetNumber();
	do
	{
		script.GetToken(); // fileName
		_tcscpy( music.szMusicFileName, script.token );
		m_aMusic.SetAtGrow( id, &music );
		id = script.GetNumber();
	} while( script.tok != FINISHED );
	m_aMusic.Optimize();

	return TRUE;
}

LPTSTR CMusicMng::GetFileName( DWORD dwId )
{
	MUSIC* pMusic = m_aMusic.GetAt( dwId );
	if( pMusic )
		return (LPTSTR)pMusic->szMusicFileName;
	return "";
}


#ifdef __MUTEBGSOUNDS
void ToggleMusicMute(bool mute)
{
	if (mute)
		SetVolume(0.0f);
	else
		SetVolume(g_Option.m_fBGMVolume);
}
#endif // __MUTEBGSOUNDS

#endif
#endif