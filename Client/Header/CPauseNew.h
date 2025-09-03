#pragma once
#include "CUIObject.h"
class CPauseNew :
    public CUIObject
{
private:
    explicit CPauseNew(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPauseNew(const CPauseNew& rhs);
    virtual ~CPauseNew();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual _bool  Action() override;

private:
    HRESULT     Add_Component();

public:
    static CPauseNew* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

