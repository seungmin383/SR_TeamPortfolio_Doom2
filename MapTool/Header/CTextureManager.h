#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CFileManager.h"
#include "CTexture.h"

class CTextureManager :
    public CBase
{
public:
    //게임오브젝트도 추가 될 수 있음
    enum class terrainDir
    { CEIL, DECO, DOOR, ENV, FLAT, FLOOR, LIGHTTILE, OBSTACLES, STEP, SWITCH, WALL, ETC, DIR_END };

    DECLARE_SINGLETON(CTextureManager)
private:
    explicit CTextureManager();
    virtual ~CTextureManager();

public:
    void                                BulkLoadTexture(LPDIRECT3DDEVICE9 pGraphicDev, string str);
    map<fs::path, CTexture*>            GetTextureGroup(string str);
    
    terrainDir                          StringConversion(string str);

private:
    map<fs::path, CTexture*> textureRepo[(_uint)terrainDir::DIR_END];

private:
    virtual void        Free();
};

