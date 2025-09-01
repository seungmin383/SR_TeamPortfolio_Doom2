#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CFileManager.h"
#include "CTextureManager.h"
#include "CTextureExplorer.h"

#include "../../Engine/Include/TerrainModelMod.h"
#include "../../Engine/Include/TerrainModelsMod.h"

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
    enum class TILEINFO { TERRAIN, WALL, CEIL };
    struct currentControlInfo
    {
        TILEINFO info = TILEINFO::TERRAIN;
        _int tileColCnt = 1;
        _int tileRowCnt = 1;

        _int vertexIdx = 0;
        vector<_vec3> vertices;
        _int vertexXCnt = 2;
        _int vertexZCnt = 2;
        _int vertexYCnt = 0;
        
        _float vertexIntervalX = 1.f;
        _float vertexIntervalZ = 1.f;
        _float vertexIntervalY = 1.f;

        _int terrainIdx = 0;
        _bool blend = false;
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
    _bool                           ControlPanelWindow(const _float& timeDelta);
    _uint                           ModifyChildWindow();
    fs::path                        SaveExplorer();

    void                            TerrainController();
    void                            ObjectController();

public:
    void                            ChangeTileInfo();

    void                            SetVertexInfo(_int tileIdx, _int vertexIdx, _vec3 vertexInfo);

    void                            GetSelectTexture();

    void                            SetDefault();

    void                            Apply();

    //save load
    void                            SaveTerrain(fs::path path, string filename);
    void                            LoadTerrain(int sceneNumber);

    void                            SaveWallAndCeil(fs::path path, string filename);
    void                            LoadWallAndCeil();

    void                            SaveItem(fs::path path, string filename);
    void                            LoadItem(int sceneNumber);

    void                            SaveMonster(fs::path path, string filename);
    void                            LoadMonster(int sceneNumber);

    void                            SetObjInfo();
    void                            SetLoadSceneTag();

    void                            InputMouse_Terrain();
    void                            InputMouse_Object();
    void                            InputMouse_Monster();

    void                            Rotate(_vec3 degree);

private:
    LPDIRECT3DDEVICE9               m_pGraphicDev;
    
    //저장직전 경로
    fs::path                        prevPath;

    //지형 관련 멤버변수
    vector<currentControlInfo>      tControlInfo;
    currentControlInfo              tCachingControlInfo;
    _vec3                           vecDegree = _vec3(0.f, 0.f, 0.f);
    CTexture*                       curTex;
    _int                            curTileIdx;

    vector<TerrainModel>            vecTerrainInfos;
    vector<TerrainModelMod>            vecTerrainInfosMod;

    //수정 목적으로 보관
    vector<CTerrainSector*>          vecSector;
    //지형 관련 멤버변수 끝

    //아이템 관련 멤버변수
    vector<GameObjectSpawnInfo>      vecItemInfos;
    GameObjectSpawnInfo              tObjectinfo;
    //아이템 관련 멤버변수 끝

    //몬스터 관련 멤버변수
    vector<GameObjectSpawnInfo>      vecMonsterInfos;
    //몬스터 관련 멤버변수 끝

    _bool                           show_demo_window = false;
    _bool                           applied = false;
    _bool                           isModifing = false;

    vector<GameObjectType>          objPrototypeTags;
    char**                          typeTags;
    vector<GameObjectType>          objIdTags;
    char**                          idTags;
    vector<GameObjectType>          objLinkModes;
    char**                          linkModeTags;


    const char*                     terrainTags[3];

    char**                          loadSceneTags;

    string                          filename;
    string                          objectTag;

    CTextureExplorer*               textureExpr;

    CCalculator*                    m_pCalculatorCom;

    _float                          pickingtime;
    _float                          pickingdelay;
    _bool                           EnablePick;

private:
    virtual void        Free();
};

