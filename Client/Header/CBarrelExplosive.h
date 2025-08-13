#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CBarrelExplosive :    public CGameObject
{
private:
    explicit CBarrelExplosive(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBarrelExplosive(const CGameObject& rhs);
    virtual ~CBarrelExplosive();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int     Update_GameObject(const float& fTimeDelta) override;
    virtual void     LateUpdate_GameObject(const float& fTimeDelta) ;
    virtual void    Render_GameObject(const _float& fTimeDelta) ;


    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;


    void            TakeDamage(int dmg);            // 총알/히트스캔이 호출

private:
    HRESULT Add_Component();
    void        Set_OnTerrain();
    void		Bill_Board();

public:
    static CBarrelExplosive* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CBarrelExplosive* Clone() override;

private:
    // 컴포넌트
    CTransform* m_pTransform = nullptr;
    CCollider* m_pCollider = nullptr;   // 구/박스 아무거나
    Engine::CTexture* m_pTextureCom;
    Engine::CCalculator* m_pCalculatorCom;
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CAnimator* m_pAnimCom;

    // 상태
    _int   m_iHP = 30;                  // 총 몇 발 맞으면 터질지
    _int   m_eAnim_Type = 0;
    _bool  m_bExploding = false;
    _int   m_iFrame_Barrel;             // 정지 프레임(필요하면)
    _int   m_iFrame_Explosion;          // 폭발 프레임
    _float m_fBlink = 0.f; // 맞았을 때 깜빡임용(선택)

private:
    enum class State { Idle, Exploding, Dead };
    State           m_state = State::Idle;
           
    float           m_animAcc = 0.f;      // 애니메이션 누적시간   
 

    Engine::CTexture* m_pTexIdle = nullptr;    // Proto_Barrel
    Engine::CTexture* m_pTexExplode = nullptr; // Proto_BarrelExp

    bool            m_damageApplied = false;   // 폭발 반경 데미지 1회 처리용
     
private:
    void            BeginExplosion();
    void            ApplyExplosionDamage();

private:
    virtual void    Free();
};

