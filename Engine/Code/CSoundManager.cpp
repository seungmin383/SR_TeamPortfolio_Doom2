#include "CSoundManager.h"


 IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
    : m_pSystem(nullptr)
{
    for (int i = 0; i < MAXCHANNEL; ++i)
        m_pChannelArr[i] = nullptr;
}

CSoundManager::~CSoundManager()
{
    Free();
}

void CSoundManager::Init()
{
    FMOD_System_Create(&m_pSystem, FMOD_VERSION);
    FMOD_System_Init(m_pSystem, MAXCHANNEL, FMOD_INIT_NORMAL, NULL);
    LoadSoundFile();
    LoadSound3D(L"EXPLOSION", TEXT("../../Client/Resource/Sound/3Dsound/DSBAREXP.wav"));
}

void CSoundManager::Update()
{
    FMOD_System_Update(m_pSystem);
}

void CSoundManager::Release()
{
}

HRESULT CSoundManager::LoadSound3D(const wstring& pSoundKey, const _tchar* pFilePath)
{
    auto iter = m_mapSound.find(pSoundKey);
    if (iter != m_mapSound.end())
        return S_OK;

    char szPath[MAX_PATH];
    size_t iConvertChars = 0;

    wcstombs_s(&iConvertChars, szPath, MAX_PATH, pFilePath, _TRUNCATE);

    FMOD_SOUND* pSound = nullptr;

    if (FMOD_OK != FMOD_System_CreateSound(m_pSystem, szPath, FMOD_3D_INVERSEROLLOFF, NULL, &pSound))
        return E_FAIL;
    
    FMOD_Sound_Set3DMinMaxDistance(pSound, 2.5f, 100.f);

    m_mapSound.emplace(pSoundKey, pSound);

    return S_OK;
}

void CSoundManager::PlaySound3D(const wstring& pSoundKey, const _vec3& vPos, _float fVolume)
{
    auto iter = m_mapSound.find(pSoundKey);
    if (iter == m_mapSound.end())
        return;

    for (_uint i = 0; i < CHANNELID::MAXCHANNEL; ++i)
    {
        FMOD_BOOL bPlay = FALSE;
        if (FMOD_OK == FMOD_Channel_IsPlaying(m_pChannelArr[i], &bPlay))
        {
            FMOD_System_PlaySound(m_pSystem, iter->second, 0, false, &m_pChannelArr[i]);

            FMOD_VECTOR soundPos = { vPos.x ,vPos.y, vPos.z };
            FMOD_VECTOR soundVel = { 0.f, 0.f, 0.f };
            FMOD_Channel_Set3DAttributes(m_pChannelArr[i], &soundPos, &soundVel);
            FMOD_Channel_SetVolume(m_pChannelArr[i], fVolume);
            return;
        }
    }
}

void CSoundManager::SetListener(const _vec3& vPos, const _vec3& vForward)
{
    FMOD_VECTOR listenerPos = { vPos.x, vPos.y, vPos.z };
    FMOD_VECTOR listenerForward = { vForward.x, vForward.y, vForward.z };
    FMOD_VECTOR listenerUp = { 0.f, 1.f ,0.f };
    FMOD_VECTOR listenerVel = { 0.f, 0.f, 0.f };

    FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
}

void CSoundManager::PlaySoundW(const std::wstring& soundKey, CHANNELID eID, float fVolume)
{
    auto iter = m_mapSound.find(soundKey);
    if (iter == m_mapSound.end())
        return;

    FMOD_BOOL isPlaying = FALSE;
    FMOD_Channel_IsPlaying(m_pChannelArr[eID], &isPlaying);

    //FMOD_Channel_Stop(m_pChannelArr[eID]);

    if (!isPlaying)
    {
        FMOD_System_PlaySound(
            m_pSystem,
            iter->second,
            0,
            FALSE,
            &m_pChannelArr[eID]
        );
    }
    else
    {
        //FMOD_Channel_Stop(m_pChannelArr[eID]);
        FMOD_System_PlaySound(
            m_pSystem,
            iter->second,
            0,
            FALSE,
            &m_pChannelArr[eID]
        );
    }

    FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
    //FMOD_System_Update(m_pSystem);
}

void CSoundManager::PlayBGM(const std::wstring& soundKey, float fVolume)
{
    auto iter = m_mapSound.find(soundKey);
    if (iter == m_mapSound.end())
        return;

    FMOD_System_PlaySound(
        m_pSystem,
        iter->second,
        0,
        FALSE,
        &m_pChannelArr[SOUND_BGM]
    );
    FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
    FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
    //FMOD_System_Update(m_pSystem);
}

void CSoundManager::StopSound(CHANNELID eID)
{
    FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundManager::StopAll()
{
    for (int i = 0; i < MAXCHANNEL; ++i)
        FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundManager::SetChannelVolume(CHANNELID eID, float fVolume)
{
    FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
    //FMOD_System_Update(m_pSystem);
}

void CSoundManager::LoadSoundFile()
{
    _finddata_t fd;
    intptr_t handle = _findfirst("../../Client/Resource/Sound/*.*", &fd);
    if (handle == -1)
        return;

    do
    {
        // 디렉터리는 건너뛰기
        if (fd.attrib & _A_SUBDIR)
            continue;

        // ANSI fd.name → std::wstring 변환
        // fd.name → wname 변환 시
        int wlen = MultiByteToWideChar(CP_ACP, 0,
            fd.name, -1,
            nullptr, 0);
        // wlen 은 널 포함 길이, 예를 들어 "foo"면 4

        // 실제 담을 길이는 wlen-1
        std::wstring wname;
        wname.resize(wlen - 1);
        MultiByteToWideChar(CP_ACP, 0,
            fd.name, -1,
            &wname[0], wlen - 1);

        // 전체 경로 ANSI로 조합
        char szFullPath[256];
        sprintf_s(szFullPath, "../../Client/Resource/Sound/%s", fd.name);

        // 사운드 로드
        FMOD_SOUND* pSound = nullptr;
        FMOD_RESULT res = FMOD_System_CreateSound(
            m_pSystem,
            szFullPath,
            FMOD_DEFAULT,
            0,
            &pSound
        );

        if (res == FMOD_OK)
        {
            // wname을 키로 바로 저장
            m_mapSound.emplace(wname, pSound);
        }

    } while (_findnext(handle, &fd) == 0);

    //FMOD_System_Update(m_pSystem);
    _findclose(handle);
}


void CSoundManager::Free()
{
    for (auto& kv : m_mapSound)
    {
        FMOD_Sound_Release(kv.second);
    }

    m_mapSound.clear();

    FMOD_System_Close(m_pSystem);
    FMOD_System_Release(m_pSystem);
    m_pSystem = nullptr;
}
