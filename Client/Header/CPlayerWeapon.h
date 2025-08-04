#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CPlayerWeapon :  public CGameObject
{
private:
    explicit CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayerWeapon(const CGameObject& rhs);
    virtual ~CPlayerWeapon();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

private:
    HRESULT     Add_Component();

private:
    Engine::CRectTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTransform* m_pTransformCom;

public:
    static CPlayerWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

