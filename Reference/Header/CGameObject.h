#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)

class CCollider;
class ENGINE_DLL CGameObject :
    public CBase
{
protected:
    explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CGameObject(const CGameObject& rhs);
    virtual ~CGameObject();

public:
    CComponent* Get_Component(COMPONENTID eID, const _tchar* pComponentTag);

    CCollider* GetCollider() { return m_pColliderCom == nullptr ? nullptr : m_pColliderCom; }

    virtual void			            OnCollision(CCollider* _pOther) {};
    virtual void			            OnCollisionEnter(CCollider* _pOther) {};
    virtual void			            OnCollisionExit(CCollider* _pOther) {};

public:
    virtual             HRESULT         Ready_GameObject();
    virtual             _int            Update_GameObject(const _float& fTimeDelta);
    virtual             void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual             void            Render_GameObject();

public:
    const bool                          Get_Dead() const { return m_bDead; }
    void                                Set_Dead(_bool _bVal) { m_bDead = _bVal; }

protected:
    map<const _tchar*, CComponent*>         m_mapComponent[ID_END];
    LPDIRECT3DDEVICE9                       m_pGraphicDev;

    CCollider*                              m_pColliderCom; // 콜라이더(충돌체)

private:
    CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);

private:
    _float                                  m_fSpeed;
    bool                                    m_bDead;

protected:
    virtual void        Free();
};

END
