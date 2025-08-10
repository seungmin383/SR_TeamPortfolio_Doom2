#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CMonster :
    public CGameObject
{
private:
    explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonster(const CGameObject& rhs);
    virtual ~CMonster();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
    HRESULT     Add_Component();

private:
    void        Chase_Player(const _float& fTimeDelta);

private:
    Engine::CTriCol* m_pBufferCom;

public:
    static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CMonster* Clone() override { return this; }

private:
    virtual void        Free();
};