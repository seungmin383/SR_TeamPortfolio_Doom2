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
    void            GetConfig();
    void            GetResourceDir();
    void            GetTerrainResourceDir();
    vector<fs::path>            GetListCurrentDir();

    //TODO: save object spawn
    //TODO: load object spawn
    
    //TODO: save monster spawn
    //TODO: load monster spawn

    void            SaveTerrain();
    void            LoadTerrain();

private:

private:
    fs::path                    m_DefaultPath = "../../Client/Asset";
    fs::path                    m_ConfigPath = "../../Client/Asset/Config.json";
    fs::path                    m_currentPath;
    ConfigModel                 m_tConfig;

private:
    virtual void Free();
};

END