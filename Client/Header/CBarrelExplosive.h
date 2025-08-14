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
    Engine::CTexture* m_pTexIdle = nullptr;    // Proto_Barrel
    Engine::CTexture* m_pTexExplode = nullptr; // Proto_BarrelExp


    // 상태
    _int   m_iHP = 30;                  // 자체 체력
    _int   m_iFrame_Barrel;             // 정지 프레임(필요하면)
    _int   m_iFrame_Explosion;          // 폭발 프레임
    _int   m_explFrameCount = 0;   // 폭발 스프라이트 총 프레임
    _int   m_idleFrameCount = 0;   // (선택) 배럴 idle 프레임
    float           m_animAcc = 0.f;      // 애니메이션 누적시간   
    bool            m_damageApplied = false;   // 폭발 반경 데미지 1회 처리용

    // 연쇄 폭발용 도화선
    bool   m_isPrimed = false;   // 이미 점화되었는지(중복 방지)
    float  m_fuseAcc = 0.f;     // 점화 후 경과 시간
    float  m_fuseTime = 0.f;     // 점화 → 실제 폭발까지 지연(초)

    // 폭발로 밀려나는 효과 적용하는 변수
    _vec3 m_vel = _vec3(0.f, 0.f, 0.f); // 수평 속도(XZ)
    float m_drag = 3.0f;                // 감쇠(마찰) 계수 [1/s], 값↑ = 빨리 멈춤
    float m_pushForce = 60.0f;           // 기본 밀어내는 힘(임펄스 크기)

private:
    enum class State { Idle, Primed, Exploding, Dead };
    State           m_state = State::Idle;

private:
    void            BeginExplosion();
    void            ApplyExplosionDamage();

    void   PrimeFuse(float delaySec); // 도화선 점화
    // 간단한 임펄스(수평) 추가용 헬퍼
    void AddImpulseXZ(const _vec3& dir, float magnitude);
private:
    virtual void    Free();
};

