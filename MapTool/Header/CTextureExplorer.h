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
    virtual ~CTextureExplorer();

public:
    static CTextureExplorer* Create();

public:
    void            DrawExplorerWindow();
    void            ChildListBox();
    void            FileListListBox();

    void            MoveParentDir();
    void            MoveDir(fs::path p);

    void            GetChildList();
    void            GetFileList();

private:
    fs::path                curDir;

    vector<fs::path>                curChildList;
    vector<fs::path>                curFileList;

    CTextureManager*        TextureRepo;

private:
    virtual void        Free();
};

