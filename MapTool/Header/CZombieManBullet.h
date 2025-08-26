#pragma once
#include "CBullet.h"

class CZombiemanBullet : public CBullet
{
private:
    explicit CZombiemanBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CZombiemanBullet(const CZombiemanBullet& rhs);
    virtual ~CZombiemanBullet();

public:
    virtual     HRESULT         Ready_GameObject()override;
    virtual     _int            Update_GameObject(const _float& fTimeDelta)override;
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta)override;
    virtual     void            Render_GameObject(const _float& fTimeDelta)override;

#pragma endregion

private:
    _bool isDead = FALSE;

private:
    HRESULT     Add_Component()override;

public:
    static CZombiemanBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

    virtual CZombiemanBullet* Clone() override;
    virtual CZombiemanBullet* Clone(CGameObject* _Owner)override;



private:


private:
    virtual void        Free()override;

};

