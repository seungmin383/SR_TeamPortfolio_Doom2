#pragma once
#include "CUIObject.h"
class CLoadingBackground :
    public CUIObject
{
private:
    explicit CLoadingBackground(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CLoadingBackground(const CLoadingBackground& rhs);
    virtual ~CLoadingBackground();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual _bool  Action() override { return true; }

private:
    HRESULT     Add_Component();
    void        Ready_Animation();

public:
    static CLoadingBackground* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    virtual void        Free();
};

