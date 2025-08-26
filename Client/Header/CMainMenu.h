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
    HRESULT Ready_Environment_Layer(LAYERID eLayerId);
    HRESULT Ready_Prototype();
    HRESULT Ready_UI_Layer(LAYERID eLayerId);

    // 입력/히트테스트
    POINT   GetMouseClientPos() const;
    bool    IsLButtonDown() const;
    void    UpdateHitRects();                   // 창 크기/텍스처 크기 기반 히트박스 재계산
    void    HandleClick(_float fTimeDelta);     // 호버/클릭 처리
    void    HandleKeyboard(_float fTimeDelta);  // ↑/↓ 이동 및 Enter 디바운스
    void    ActivateFocused();                  // 선택 실행

    // 사운드(나중에 켜기)
    void    PlaySfx_Move();
    void    PlaySfx_Select();

private:
    CLoading* m_pLoading = nullptr;
    CBackGround* m_pBackground = nullptr;

    // UI 상태
    RECT          m_rcStart = {};
    RECT          m_rcExit = {};
    int           m_hover = -1;             // -1: none, 0: start, 1: exit
    int           m_focus = 0;              // 키보드 포커스(기본 START)
    bool          m_prevLDown = false;      // 클릭 엣지 감지

    _float m_inputLock = 0.2f;              // 키 입력 디바운스


    // --- 텍스처 실제 크기와 화면상 위치 캐시 ---
    D3DXVECTOR2         m_posStart = { 0.f, 0.f };
    D3DXVECTOR2         m_posExit = { 0.f, 0.f };
    D3DXVECTOR2         m_sizeStart = { 0.f, 0.f };
    D3DXVECTOR2         m_sizeExit = { 0.f, 0.f };
    D3DXVECTOR2         m_sizeSkull = { 0.f, 0.f };

    struct KeyPrev {
        bool up = false, down = false, w = false, s = false, enter = false, esc = false;
    } m_keyPrev;

    LPD3DXSPRITE        m_pUISprite = nullptr;
    LPDIRECT3DTEXTURE9  m_pLogoTex = nullptr;
    LPDIRECT3DTEXTURE9  m_pExitTex = nullptr;
    LPDIRECT3DTEXTURE9  m_pHoverTex = nullptr;

private:
    virtual void  Free() override;
};

