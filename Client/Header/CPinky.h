#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CPinky :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private:
    explicit CPinky(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPinky(const CMonsterBase& rhs);
    virtual ~CPinky();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    virtual void         Attack(const _float fTimeDelta)override;
    virtual void            Drop_Item()override;    

#pragma endregion
private:
    HRESULT     Add_Component()override;

public:
    static CPinky* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CPinky* Clone() override;

private:
    virtual void        Free();
};

