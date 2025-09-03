#pragma once
#include "CUIObject.h"
class CLoadingSpinner :
    public CUIObject
{
private:
    explicit CLoadingSpinner(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CLoadingSpinner(const CLoadingSpinner& rhs);
    virtual ~CLoadingSpinner();

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
    static CLoadingSpinner* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CGameObject* Clone() override;

private:
    CRectSpriteTex*     m_pSpriteBuffer;
    CAnimator*          m_pAnimatorCom;

private:
    virtual void        Free();
};

