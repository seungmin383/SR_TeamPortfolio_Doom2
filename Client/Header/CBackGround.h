#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CBackGround :
    public CGameObject
{
private:
    explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBackGround(const CGameObject& rhs);
    virtual ~CBackGround();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

private:
    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);

private:
    Engine::CTriCol*        m_pBufferCom;
    Engine::CTransform*     m_pTransformCom;

    Engine::CRectCol* m_pMonsterBufferCom;
    Engine::CTransform* m_pMonsterTransformCom;

public:
    static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

