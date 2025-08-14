#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CFileManager.h"
#include "CTextureManager.h"

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

class CTextureExplorer :
    public CBase
{
private:
    explicit CTextureExplorer();
    explicit CTextureExplorer(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CTextureExplorer();

public:
    static CTextureExplorer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    void            DrawExplorerWindow();
    void            ChildListBox();
    void            FileListListBox();
    void            TextureList();

    void            MoveParentDir();
    void            MoveDir(fs::path p);
    void            MoveTerrainResourceDir();

    void            GetChildList();
    void            GetFileList();

    void            BulkLoadTexture();
    void            GetTextures();

    string          WideToUtf8(const _tchar* wc);

private:
    LPDIRECT3DDEVICE9               m_pGraphicDev;

    fs::path                        curDir;
    fs::path                        selectedTexture;

    vector<fs::path>                curChildList;
    vector<fs::path>                curFileList;

    CTextureManager*                TextureRepo;
    map<fs::path, CTexture*>        curTextures;
    _bool                           cacheCheck[(_uint)CTextureManager::terrainDir::DIR_END] = { 0, };

private:
    virtual void        Free();
};

