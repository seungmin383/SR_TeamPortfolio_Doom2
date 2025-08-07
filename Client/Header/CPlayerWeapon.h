#pragma once
#include "CCharacter.h"
#include "CPrototypeManager.h"

class CPlayerWeapon :  public CCharacter
{
public:
    enum WEAPON_ID {PISTOL_IDLE, PISTOL_SHOT, SHOTGUN, CHAINGUN, ROCKET, BFG, WEAPON_END};
    enum WEAPON_STATE {IDLE, ATTACK, SWAP};

private:
    explicit CPlayerWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayerWeapon(const CCharacter& rhs);
    virtual ~CPlayerWeapon();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

    /// <summary>
    /// 애니메이션 세팅
    /// </summary>
    void                        Ready_Animation();

private:
    HRESULT     Add_Component();
    void        Key_Input();
    void        Mouse_Input();

    void Movement() override;
    void Attack() override;

private:
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CAnimator* m_pAnimatorCom;

    WEAPON_ID       m_eWeaponId;
    WEAPON_STATE    m_eWeaponState;

public:
    static CPlayerWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

