#pragma once
#include "CUIObject.h"

class CMainMenuHover :
    public CUIObject
{
private:
    explicit CMainMenuHover(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMainMenuHover(const CMainMenuHover& rhs);
    virtual ~CMainMenuHover();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual void  Action() override { return; }

private:
    HRESULT     Add_Component();

public:
    static CMainMenuHover* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

