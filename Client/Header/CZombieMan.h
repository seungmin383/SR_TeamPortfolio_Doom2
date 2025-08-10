#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CZombieMan :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private :
    explicit CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CZombieMan(const CMonsterBase& rhs);
    virtual ~CZombieMan();

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
    void        Key_Input(const _float& fTimeDelta);

public:
    static CZombieMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();




};

