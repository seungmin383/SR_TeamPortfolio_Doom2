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

    // 사운드(나중에 켜기)
    void    PlaySfx_Move();
    void    PlaySfx_Select();

private:
    virtual void  Free() override;
};

