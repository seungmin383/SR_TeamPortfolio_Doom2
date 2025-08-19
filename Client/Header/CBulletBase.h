#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "IMovementStrategy.h"
#include "IImpactStrategy.h"


class CBulletBase :public CGameObject
{
protected:
    explicit CBulletBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBulletBase(const CBulletBase& rhs);
    virtual ~CBulletBase();

public:
    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

    virtual  HRESULT Ready_GameObject(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos);
    virtual  _int   Update_GameObject(const _float& fTimeDelta)override;

private:
    virtual HRESULT     Add_Component();

public:
    // MovementStrategy
    IMovementStrategy* GetMovementStrategy() const { return Movement_Strategy; }
    void SetMovementStrategy(IMovementStrategy* pStrategy) { Movement_Strategy = pStrategy; }

    // ImpactStrategy
    IImpactStrategy* GetImpactStrategy() const { return Impact_Strategy; }
    void SetImpactStrategy(IImpactStrategy* pStrategy) { Impact_Strategy = pStrategy; }

protected:
    Engine::CCalculator*                m_pCalculatorCom;
    Engine::CRectSpriteTex*         m_pBufferCom;
    Engine::CTexture*                   m_pTextureCom;
    Engine::CAnimator*                  m_pAnimCom;

    IMovementStrategy* Movement_Strategy;
    IImpactStrategy* Impact_Strategy;

protected:
    //m_fSpeed;     //게임 오브젝트에 있음
    //m_bDead;      //게임 오브젝트에 있음
    _int        m_iDamage;
    _vec3       m_vDir;
    MONSTER_BULLET_TYPE m_BulletType;

public:
    virtual void Create(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos);
protected:
    virtual void Free();
};

