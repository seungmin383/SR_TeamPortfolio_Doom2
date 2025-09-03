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
    virtual     _int             Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

  /*  virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;*/

    virtual void         Attack(const _float fTimeDelta)override;
    void Drop_Item()override;
    _float m_fSound = 1.f;
#pragma endregion

private:

    HRESULT     Add_Component()override;

public:
    static CZombieMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    CZombieMan* Clone() override;
    CZombieMan* Clone(_vec3 InitPos, _vec3 InitLook);

private:
    virtual void        Free();




};

