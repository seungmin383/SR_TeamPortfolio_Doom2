#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
class CObject :    public CGameObject
{
private:
    explicit CObject(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CObject(const CGameObject& rhs);
    virtual ~CObject();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
    HRESULT     Add_Component();

private:
    Engine::CTriCol* m_pBufferCom;

public:
    static CObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

