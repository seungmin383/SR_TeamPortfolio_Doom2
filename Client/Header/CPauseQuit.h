#pragma once
#include "CUIObject.h"
class CPauseQuit :
    public CUIObject
{
private:
    explicit CPauseQuit(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPauseQuit(const CPauseQuit& rhs);
    virtual ~CPauseQuit();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual _bool  Action() override;

private:
    HRESULT     Add_Component();

public:
    static CPauseQuit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

