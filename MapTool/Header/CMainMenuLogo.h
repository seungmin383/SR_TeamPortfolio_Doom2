#pragma once
#include "CUIObject.h"

class CMainMenuLogo :
    public CUIObject
{
private:
    explicit CMainMenuLogo(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMainMenuLogo(const CMainMenuLogo& rhs);
    virtual ~CMainMenuLogo();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual void  Action() override;

private:
    HRESULT     Add_Component();

public:
    static CMainMenuLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

