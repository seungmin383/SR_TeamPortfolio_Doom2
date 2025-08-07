#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CSkyBox :
    public CGameObject
{
private:
    explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CSkyBox(const CSkyBox& rhs);
    virtual ~CSkyBox();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

private:
    HRESULT     Add_Component();

private:
    Engine::CCubeTex* m_pBufferCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;

public:
    static CSkyBox* Create(LPDIRECT3DDEVICE9 pGarphicDev);

private:
    virtual void        Free();
};

