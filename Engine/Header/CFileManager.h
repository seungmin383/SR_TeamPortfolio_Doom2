#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "../../Engine/Include/ConfigModel.h"
#include "../../Engine/Include/TerrainModels.h"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

BEGIN(Engine)

class ENGINE_DLL CFileManager :
    public CBase
{
    DECLARE_SINGLETON(CFileManager)
private:
    explicit CFileManager();
    virtual ~CFileManager();

public:
    fs::path        GetCurrentDir();
    fs::path        GetCanonicalDir();
    void            SetProjectDir();
    void            MoveProjectDir();
    void            MoveParentDir();
    void            MoveDir(fs::path p);

    void                        GetConfig();
    fs::path                    GetResourceDir();
    fs::path                    GetTerrainResourceDir();
    vector<fs::path>            GetListCurrentDir();
    vector<fs::path>            GetListChildDir();

    //TODO: save object spawn
    //TODO: load object spawn
    
    //TODO: save monster spawn
    //TODO: load monster spawn

    void            SaveTerrain();
    void            LoadTerrain();

private:

private:
    fs::path                    m_projectPath;
    fs::path                    m_DefaultPath = "../../Client/Asset";
    fs::path                    m_ConfigPath = "../../Client/Asset/Config.json";
    fs::path                    m_currentPath;
    ConfigModel                 m_tConfig;
    vector<fs::path>            m_vecChildDir;

private:
    virtual void Free();
};

END