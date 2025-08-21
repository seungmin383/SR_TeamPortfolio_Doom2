#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "../../Engine/Include/StageInfo.h"
#include "../../Engine/Include/ConfigModel.h"
#include "../../Engine/Include/TerrainModels.h"
#include <filesystem>
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
    void            SetProjectDir();
    void            MoveProjectDir();
    void            MoveParentDir();
    void            MoveDir(fs::path p);

    void                        LoadConfig();
    ConfigModel                 GetConfig() { return m_tConfig; }
    fs::path                             GetResourceDir();
    fs::path                            GetBaseDir() { return m_basePath; }
    fs::path                            GetAssetDir() { return m_DefaultPath; }
    fs::path                            GetTerrainResourceDir();
    vector<fs::path>            GetListCurrentDir();
    vector<fs::path>            GetListChildDir();

    //TODO: save object spawn
    //TODO: load object spawn
    
    //TODO: save monster spawn
    //TODO: load monster spawn

    void                              SaveTerrain(fs::path path, string filename, vector<TerrainModel> datas);
    TerrainModels                     LoadTerrain();

    void                              SaveItem(fs::path path, string filename, vector<GameObjectSpawnInfo> datas);
    vector<GameObjectSpawnInfo>       LoadItem();

private:

private:
    fs::path                    m_basePath;
    fs::path                    m_projectPath;
    fs::path                    m_DefaultPath = "../../Client/Asset";
    fs::path                    m_ConfigPath = "../../Client/Asset/Config.json";
    ConfigModel                 m_tConfig;

    vector<StageInfo>           m_vecStageInfos;

private:
    virtual void Free();
};

END