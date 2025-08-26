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

    virtual void         Attack(const _float fTimeDelta)override;
    
    //난수를 이용해서 패턴 규칙 만들기
    virtual void Calc_Pattern();

    virtual void Fire(const _float fTimeDelta);
    virtual void Pattern1(const _float fTimeDelta);
    virtual void Pattern2(const _float fTimeDelta);
    virtual void Pattern3(const _float fTimeDelta);
    virtual void Pattern4(const _float fTimeDelta);

#pragma endregion

private:
    HRESULT     Add_Component()override;

    uint16_t  m_iPattern = 0;
    _int m_PattenArr[8] = {};

public:
    static CSpiderMastermind* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CSpiderMastermind* Clone() override;

private:
    virtual void        Free();




};

