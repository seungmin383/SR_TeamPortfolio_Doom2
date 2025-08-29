#pragma once
#include "CUIObject.h"

class CMainMenuExit :
    public CUIObject
{
private:
    explicit CMainMenuExit(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMainMenuExit(const CMainMenuExit& rhs);
    virtual ~CMainMenuExit();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

private:
    HRESULT     Add_Component();

public:
    static CMainMenuExit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override { return this; }

private:
    virtual void        Free();
};

