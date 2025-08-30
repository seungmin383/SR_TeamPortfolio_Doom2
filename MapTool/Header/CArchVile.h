#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CArchVile :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private:
    explicit CArchVile(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CArchVile(const CMonsterBase& rhs);
    virtual ~CArchVile();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);
    
    virtual void         Attack(const _float fTimeDelta)override;

private:
    HRESULT     Add_Component()override;

public:
    static CArchVile* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CArchVile* Clone() override;

private:
    virtual void        Free();
};

