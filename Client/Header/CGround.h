#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CGround :
    public CGameObject
{
private:
    explicit CGround(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CGround(const CGameObject& rhs);
    virtual ~CGround();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

private:
    HRESULT     Add_Component();

private:
    Engine::CTileGround* m_pBufferCom;
    Engine::CTransform* m_pTransformCom;

public:
    static CGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};