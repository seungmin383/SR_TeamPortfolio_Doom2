#pragma once
#include "CCharacter.h"
#include "CPrototypeManager.h"

#define ANIM_MOVE 0
#define ANIM_ATK 1
#define ANIM_NDEAD 2
#define ANIM_GDEAD 3
#define ANIM_HIT 4

class CMonsterBase :public CCharacter
{
    //몬스터 자료형을 통일시키고 싶지만 GameObject로 통일하고 싶진 않아서 만든 클래스 
protected:
    explicit CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonsterBase(const CMonsterBase& rhs);
    virtual ~CMonsterBase();

public:
    virtual _vec3 Set_Move_Direction(_vec3* _Look, _float _DelayTime, _float _fTimedelta);

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int                  Update_GameObject(const _float& fTimeDelta);
    virtual     void                 LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void                 Render_GameObject(const _float& fTimeDelta);

    virtual void			            OnCollision(CCollider* _pOther) override;
    virtual void			            OnCollisionEnter(CCollider* _pOther) override;
    virtual void			            OnCollisionExit(CCollider* _pOther) override;

    //Thinker 개편
    virtual void Thinker(const _float& fTimeDelta);                                 //생각하는 함수 -> 몬스터 AI
    virtual _int Perception(const _float& fTimeDelta);                               //인지하는 함수 -> 몬스터가 어떤 행동을 할지 상태를 지정해주는 함수
        virtual _bool Check_Dead();                                                             // 죽었는가?
        virtual _bool Check_Target();                                                           //시야에 플레이어가 있는지 확인하는 함수
        virtual _bool Check_Shooting_Sound();                                                           //시야에 플레이어가 있는지 확인하는 함수
        virtual void Check_Hit(const _float& fTimeDelta);                                                                //맞았는가?
        virtual void Calc_Hp();
        virtual _bool Check_Attack(const _float& fTimeDelta);

    virtual void  Action(const _float& fTimeDelta, _int _Thought);          //행동하는 함수
        virtual void Monster_Look_Player(_vec3* _vLook);
        virtual void Monster_Fire();



    //기능 Function (기능 기능?)
   virtual  void        Set_OnTerrain();
   virtual void         Movement(const _float fTimeDelta);
   virtual void         Attack(const _float fTimeDelta);

   virtual _vec3 Get_PlayerPos();
   virtual _float Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos);
   virtual _float Calc_AngleToAttacker(_vec3 vLook, _vec3 vPos, _vec3 vAttackerPos);

   virtual _int Calc_AnimState_byPlayer(_vec3 vLook, _vec3 vPos);
   virtual _int Calc_AnimState_byAttacker(_vec3 vLook, _vec3 vPos, _vec3 vAttackerPos);

   virtual void Debug_Triangle(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2);
   virtual void Debug_Rectangle(_vec3 _LeftTop, _vec3 _RightTop, _vec3 _RightBottom, _vec3 _LeftBottom);

   virtual void Bill_Board();

   virtual void Drop_Item();

   //Setting Function
   virtual HRESULT     Add_Component();

public:
    const _vec3 Get_MoveLook()const { return m_vMoveLook; };

protected:
    Engine::CCalculator*            m_pCalculatorCom;
    Engine::CRectSpriteTex*      m_pBufferCom;
    Engine::CTexture*                m_pTextureCom;
    Engine::CAnimator*             m_pAnimCom;

protected:
    _int m_iCurThought = 0;                 //현재 ai 상태
    _int m_iPreThought = 0;                 //이전 ai 상태

    _int m_eCurAnimState = 0;             //현재 애니메이션 상태 -> 텍스쳐 변환용
    _int m_ePreAnimState = 0;               //이전 애니메이션 상태

    _int m_eAnim_Type = 0;

    _vec3 m_vMoveLook;
    _vec3 m_vtmpLook;

    _float m_fSightAngle = 0.f;
    _float m_fSightDistance = 0.f;


    _float m_fAtkDelay = 0.f; 
    _float m_fMoveDelay = 0.f;
    _float m_fChaseDelay = 0.f;
    _float m_fHitDelay = 0.f;

    _int m_AtkEnterCnt = 0;         //공격체크 함수에 몇번 들어갔는지 확인
    _int m_HitEnterCnt = 0;          //히트체크 함수에 몇번 들어갔는지 확인

    _bool m_isMove = false;
    _bool m_isAtk = false;

    //Thinker Variable
    _bool m_isDetect_Player         = false;                //시야에 플레이어가 들어왔는가?
    _bool m_isDetected_Player     = false;           //시야에 플레이어가 들어온 적이 있는가?
    _bool m_isRecognize_Player   = false;          // 한번이라도 플레이어를 본 적이 있는가

    _bool m_isHit = false;                                 //총알에 맞았는지?
    _int    m_isDead = false;                             //죽었는가?
    _bool m_isNormalDead = false;                  //그냥 죽었는가?
    _bool m_isGibbingDead = false;                 //폭사했는가?

    // 적 처리 카운트 플래그 추가(08/24 김홍인)
    _bool m_bCountedDeath = false;

    //Decision Variable

    //기능 Function Variable
    _vec3   m_PlayerPos;

    _uint m_iGibDeadPlus = 0;
    _uint m_iNorDeadPlus = 0;

    _bool m_isPinky = FALSE;
    _bool m_isShoot = FALSE;

    OBJPROTOTYPE m_iMonsterType = OBJPROTOTYPE::OBJPROTOTYPE_END;

    _float m_fTargetAngle = 0.f;
    _float m_fCurAngle = 1.f;

    CGameObject* m_pAttackTarget = nullptr; // nullptr 이면 Player

    _int m_iPreHp = m_iGameHp;

    //보스 패턴용 변수
    _int m_AtkCnt = 0;
    _bool m_isFirstEnt = FALSE;



    // --- 드랍 1회만 실행되도록 하는 가드 ---
    _bool m_bDropDone = false;
public:
    CGameObject* Get_AttackTarget() const { return m_pAttackTarget; }

    void Set_AttackTarget(CGameObject* pTarget) { m_pAttackTarget = pTarget; }

protected:
    virtual void Free();

};

