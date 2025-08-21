#pragma once
#include "CBulletBase.h"

class CZombieManBullet :public CBulletBase
{
protected:
    explicit CZombieManBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CZombieManBullet(const CZombieManBullet& rhs);
    virtual ~CZombieManBullet();


    virtual  HRESULT Ready_GameObject(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)override;
    virtual  _int   Update_GameObject(const _float& fTimeDelta)override;

public:
    virtual void Create(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)override;

protected:
    virtual void Free()override;
};

