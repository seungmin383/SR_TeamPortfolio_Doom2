#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

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

    virtual  HRESULT Ready_GameObject();    virtual  _int   Update_GameObject(const _float& fTimeDelta)override;

private:
    virtual HRESULT     Add_Component();

protected:
    Engine::CCalculator*                m_pCalculatorCom;
    Engine::CRectSpriteTex*         m_pBufferCom;
    Engine::CTexture*                   m_pTextureCom;
    Engine::CAnimator*                  m_pAnimCom;

protected:
    //m_fSpeed;     //게임 오브젝트에 있음
    //m_bDead;      //게임 오브젝트에 있음
    _int        m_iDamage;
    _vec3       m_vDir;

protected:
    virtual void Free();
};

