#pragma once
#include "CBase.h"
#include "Engine_Define.h"

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

class CImGuiManager :
    public CBase
{
    DECLARE_SINGLETON(CImGuiManager)
private:
    explicit CImGuiManager();
    virtual ~CImGuiManager();

public:
    void        Init_ImGui(LPDIRECT3DDEVICE9 pGraphicDev);
    void        Render_ImGui();
    void        ResetDevice();

    void        DirExplorerWindow();

    void        DemoWindow();
    void        DemoAnotherWindow();

private:
    virtual void        Free();

private:
    LPDIRECT3DDEVICE9       m_pGraphicDev;
    ImVec4                  clear_color;

    bool show_demo_window = true;
    bool show_another_window = false;
};

