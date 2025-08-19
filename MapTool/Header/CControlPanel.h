#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CFileManager.h"
#include "CTextureManager.h"
#include "CTextureExplorer.h"

#include "CTerrainSector.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"

#ifdef _MSC_VER
#pragma pop_macro("new")
#endif

class CControlPanel :
    public CBase
{
    enum class TILEINFO { FLOOR, WALL, CEIL };
    struct currentControlInfo
    {
        TILEINFO info = TILEINFO::FLOOR;
        _int tileColCnt = 1;
        _int tileRowCnt = 1;

        _int vertexIdx = 0;
        vector<_vec3> vertices;
        _int vertexXCnt = 2;
        _int vertexZCnt = 2;
        _int vertexYCnt = 0;
        
        _int vertexInterval = 1.f;

        _int terrainIdx = 0;
        fs::path currentTextureTag = "";

        _vec3 pos = _vec3(0.f, 0.f, 0.f);

        //ImGuiTexId texId;
    };
private:
    explicit CControlPanel();
    explicit CControlPanel(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr);
    virtual ~CControlPanel();

public:
    static CControlPanel* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr);

    //TODO: ㅈ같은코드정리
public:
    _bool                           ControlPanelWindow();
    _uint                           ModifyChildWindow();
    fs::path                        SaveExplorer();

    void                            TerrainController();
    void                            ObjectController();

public:
    void                            ChangeTileInfo();

    void                            SetVertexInfo(_int tileIdx, _int vertexIdx, _vec3 vertexInfo);

    void                            GetSelectTexture();

    void                            SetDefault();

    /// <summary>
    /// 현재 조정 완료하고 벡터에 추가
    /// </summary>
    void                            Apply();

    //save
    void                            SaveTerrain(fs::path path, string filename);
    void                            LoadTerrain();

    void                            SaveWallAndCeil(fs::path path, string filename);
    void                            LoadWallAndCeil();

    void                            SaveItem(fs::path path, string filename);
    void                            LoadItem();

    void                            SaveMonster(fs::path path, string filename);
    void                            LoadMonster();

    void                            SetObjInfo();

private:
    LPDIRECT3DDEVICE9               m_pGraphicDev;
    
    vector<currentControlInfo>      tControlInfo;
    currentControlInfo              tCachingControlInfo;
    _vec3                           vecCachingVtx = _vec3(0.f, 0.f, 0.f);
    CTexture*                       curTex;
    _uint                           curTileIdx;

    //모델 벡터는 저장할 정보임
    vector<TerrainModel>            vecTerrainInfos;
    //TODO:
    //vector<CeilModel>            vecTerrainInfos;
    //vector<FloorMOdel>            vecTerrainInfos;

    //수정 목적으로 보관
    vector<CTerrainSector*>          vecSector;

    vector<GameObjectSpawnInfo>      vecItemInfos;

    vector<GameObjectSpawnInfo>      vecMonsterInfos;

    //trash
    _bool                           show_demo_window = false;
    _bool                           applied = false;
    _bool                           isModifing = false;

    vector<GameObjectType>          objPrototypeTags;
    char**                          typeTags;
    vector<GameObjectType>          objIdTags;
    char**                          idTags;

    string                          filename;

    CTextureExplorer*               textureExpr;

private:
    virtual void        Free();
};

