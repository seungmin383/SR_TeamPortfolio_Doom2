#pragma once
#include "CGameObject.h"
#include"CPrototypeManager.h"

class CBulletBase :public CGameObject
{
protected:
    explicit CBulletBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBulletBase(const CBulletBase& rhs);
    virtual ~CBulletBase();


public:
    virtual HRESULT     Add_Component();

protected:
    Engine::CCalculator*           m_pCalculatorCom;
    Engine::CRectSpriteTex*     m_pBufferCom;
    Engine::CTexture*               m_pTextureCom;
    Engine::CAnimator*            m_pAnimCom;

protected:
    //m_fSpeed;     //게임 오브젝트에 있음
    //m_bDead;      //게임 오브젝트에 있음

protected:
    virtual void Free();
};

