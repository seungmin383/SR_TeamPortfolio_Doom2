#pragma once
#include "CCharacter.h"
#include "CPrototypeManager.h"

class CPlayerWeapon :  public CCharacter
{
public:
    enum WEAPON_ID 
    {
        PISTOL,
        SHOTGUN,
        CHAINGUN,
        ROCKET,
        BFG,
        ID_END
    };

    enum WEAPON_STATE
    {
        IDLE = 0,
        SHOT = 5,
        SWAP = 6,
        STATE_END
    };

private:
    explicit CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayerWeapon(const CCharacter& rhs);
    virtual ~CPlayerWeapon();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    /// <summary>
    /// 애니메이션 세팅
    /// </summary>
    void                        Ready_Animation();

private:
    HRESULT     Add_Component();
    void        Key_Input();
    void        Mouse_Input();
    void        Weapon_Swap(const _float& fTimeDelta);


private:
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CAnimator* m_pAnimatorCom;

    WEAPON_ID       m_eWeaponId;
    WEAPON_STATE    m_eWeaponState;
    _float          m_fSwapTime;

public:
    static CPlayerWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CPlayerWeapon* Clone() override;

private:
    virtual void        Free();
};

