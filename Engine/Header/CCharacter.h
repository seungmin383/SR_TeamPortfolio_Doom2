#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCharacter :public CGameObject
{
protected:
    explicit CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCharacter(const CCharacter& rhs);
    virtual ~CCharacter();

public:
    virtual             HRESULT         Ready_GameObject();
    virtual             _int            Update_GameObject(const _float& fTimeDelta);
    virtual             void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual             void            Render_GameObject(const _float& fTimeDelta);

#pragma region OVERRIDE
public:
    virtual void Movement(const _float& fTimeDelta)PURE;
    virtual void Attack()PURE;
#pragma endregion

#pragma region GETTER / SETTER
public:
    _int GetHp() const { return m_iHp; }
    void SetHp(_int iHp) { m_iHp = iHp; }

    _int GetAtkDmg() const { return m_iAtkDmg; }
    void SetAtkDmg(_int iAtkDmg) { m_iAtkDmg = iAtkDmg; }

    _float GetAtkSpeed() const { return m_fAtkSpeed; }
    void SetAtkSpeed(_float fAtkSpeed) { m_fAtkSpeed = fAtkSpeed; }
#pragma endregion

protected:
    _int    m_iHp;
    _int    m_iAtkDmg;
    _float m_fAtkSpeed;


protected:
    virtual void Free();
};

END