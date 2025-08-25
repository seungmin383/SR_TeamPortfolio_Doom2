#pragma once
#include "CBullet.h"

class CBossBullet : public CBullet
{
private:
    explicit CBossBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBossBullet(const CBossBullet& rhs);
    virtual ~CBossBullet();

public:
    virtual     HRESULT         Ready_GameObject()override;
    virtual     _int            Update_GameObject(const _float& fTimeDelta)override;
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta)override;
    virtual     void            Render_GameObject(const _float& fTimeDelta)override;

#pragma endregion

    virtual void Check_Anim_Is_Finish();
private:
    _uint Enter_Func_Cnt = 0;
    _bool isFinAnim = FALSE;
    _bool isDead = FALSE;

private:
    HRESULT     Add_Component()override;

public:
    static CBossBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CBossBullet* Clone() override;
    virtual CBossBullet* Clone(CGameObject* _Owner)override;

private:
    virtual void        Free()override;

};

