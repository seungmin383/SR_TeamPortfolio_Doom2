#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CBackGround :
    public CGameObject
{
private:
    explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBackGround(const CGameObject& rhs);
    virtual ~CBackGround();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

private:
    HRESULT     Add_Component();

private:
    Engine::CRectTex*        m_pBufferCom;
    Engine::CTexture*       m_pTextureCom;

public:
    static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CBackGround* Clone() override { return this; }

private:
    virtual void        Free();
};

