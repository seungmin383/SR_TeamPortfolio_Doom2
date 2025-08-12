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
        SWAP = 10,
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
    void        Key_Input(const _float& fTimeDelta);
    void        Mouse_Input();

    void        Weapon_Swap(const _float& fTimeDelta);
    void        Weapon_Bobbing(bool bIsMoving, const _float& fTimeDelta);


private:
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CAnimator* m_pAnimatorCom;

    WEAPON_ID       m_eCurWeaponId;
    WEAPON_ID       m_eNextWeaponId;
    WEAPON_STATE    m_eWeaponState;

    bool            m_bIsMoving;
    bool            m_bBobForward;      // 삼각함수 매개변수 진행방향 바꾸는 변수

    _float          m_fBobSpeed;        // 삼각함수의 매개변수로 들어가는 변수
    _float          m_fBobAngle;
    _float          m_fSwapTime;

    _vec3           m_vScale;
    _vec3           m_vPos;             // 총의 처음 위치를 저장하는 변수
    _vec3           m_vBobPos;          // 총의 위치를 기록하기 위한 변수

public:
    static CPlayerWeapon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CPlayerWeapon* Clone() override;

private:
    virtual void        Free();
};

