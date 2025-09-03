#pragma once
#include "CUIObject.h"
class CPausePause :
    public CUIObject
{
private:
    explicit CPausePause(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPausePause(const CPausePause& rhs);
    virtual ~CPausePause();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual _bool  Action() override;

private:
    HRESULT     Add_Component();

public:
    static CPausePause* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

