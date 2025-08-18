#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CShotgunGuy :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private:
    explicit CShotgunGuy(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CShotgunGuy(const CMonsterBase& rhs);
    virtual ~CShotgunGuy();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;


#pragma endregion

private:
    HRESULT     Add_Component()override;

public:
    static CShotgunGuy* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CShotgunGuy* Clone() override;

private:
    virtual void        Free();




};

