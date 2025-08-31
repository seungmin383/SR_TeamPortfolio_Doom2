#pragma once
#include "CUIObject.h"

class CUICamera :
    public CUIObject
{
private:
    explicit CUICamera(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CUICamera(const CUICamera& rhs);
    virtual ~CUICamera();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual void  Action() override { return; }

private:
    HRESULT     Add_Component();

public:
    static CUICamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    _matrix             m_matWorld;

private:
    virtual void        Free();
};

