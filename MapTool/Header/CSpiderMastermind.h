#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CSpiderMastermind :public CMonsterBase
{
private:
    explicit CSpiderMastermind(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CSpiderMastermind(const CMonsterBase& rhs);
    virtual ~CSpiderMastermind();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    void Set_OnTerrain()override;
#pragma endregion

private:
    HRESULT     Add_Component()override;

public:
    static CSpiderMastermind* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CSpiderMastermind* Clone() override;

private:
    virtual void        Free();




};

