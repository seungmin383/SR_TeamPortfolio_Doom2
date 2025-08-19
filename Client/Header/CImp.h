#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CImp :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private:
    explicit CImp(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CImp(const CMonsterBase& rhs);
    virtual ~CImp();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);


#pragma endregion

private:
    HRESULT     Add_Component()override;

public:
    static CImp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CImp* Clone() override;

private:
    virtual void        Free();




};

