#pragma once
#include "CUIObject.h"
class CCrosshair :  public CUIObject
{
private:
    explicit CCrosshair(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCrosshair(const CCrosshair& rhs);
    virtual ~CCrosshair();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual void  Action() override { return; }

private:
    HRESULT     Add_Component();

public:
    static CCrosshair* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();

};