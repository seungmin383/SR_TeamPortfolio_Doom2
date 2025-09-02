#pragma once
#include "CBullet.h"

class CBossBasicBullet : public CBullet
{
private:
    explicit CBossBasicBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBossBasicBullet(const CBossBasicBullet& rhs);
    virtual ~CBossBasicBullet();

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
    static CBossBasicBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CBossBasicBullet* Clone() override;
    virtual CBossBasicBullet* Clone(CGameObject* _Owner)override;



private:


private:
    virtual void        Free()override;

};

