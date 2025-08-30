#pragma once
#include "CScene.h"
#include "CLoading.h"

class CBackGround;

class CMainMenu :    public CScene
{
public:
    explicit CMainMenu(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CMainMenu();

public:
    virtual HRESULT Ready_Scene() override;
    virtual _int    Update_Scene(const _float& fTimeDelta) override;
    virtual void    LateUpdate_Scene(const _float& fTimeDelta) override;
    virtual void    Render_Scene(const _float& fTimeDelta) override;
    virtual HRESULT Load_PreSet() override;
    virtual CMainMenu* Create() override;

public:
    static CMainMenu* Standby(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    HRESULT Ready_Prototype();
    HRESULT Ready_UI_Layer(LAYERID eLayerId);

    // 입력/히트테스트
    void    HandleKeyboard(_float fTimeDelta);  // ↑/↓ 이동 및 Enter 디바운스
    void    ActivateFocused();                  // 선택 실행

    // 사운드(나중에 켜기)
    void    PlaySfx_Move();
    void    PlaySfx_Select();

private:
    // UI 상태
    int           m_hover = -1;             // -1: none, 0: start, 1: exit
    int           m_focus = 0;              // 키보드 포커스(기본 START)
    bool          m_prevLDown = false;      // 클릭 엣지 감지

private:
    virtual void  Free() override;
};

