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
        virtual _bool Check_Hit();                                                                //맞았는가?
        virtual _bool Check_Attack(const _float& fTimeDelta);

    virtual void  Action(const _float& fTimeDelta, _int _Thought);          //행동하는 함수
        virtual void Monster_Look_Player(_vec3* _vLook);



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

   //Setting Function
   virtual HRESULT     Add_Component();

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

    _float m_fSightAngle;
    _float m_fSightDistance;


    _float m_fAtkDelay; 
    _float m_fMoveDelay;
    _float m_fChaseDelay;

    _int m_EnterCnt = 0;

    _bool m_isMove;
    _bool m_isAtk;


    //Thinker Variable
    _bool m_isDetect_Player;                //시야에 플레이어가 들어왔는가?
    _bool m_isDetected_Player;          //시야에 플레이어가 들어온 적이 있는가?
    _bool m_isEverSeen_Player;          // 한번이라도 플레이어를 본 적이 있는가

    _bool m_isHit;                                 //총알에 맞았는지?
    _int    m_isDead;                             //죽었는가?
    _bool m_isNormalDead;                  //그냥 죽었는가?
    _bool m_isGibbingDead;                 //폭사했는가?

    //Decision Variable
    _ulong Monster_Stat_Bit;

    //기능 Function Variable
    _vec3   m_PlayerPos;

    _uint m_iGibDeadPlus;
    _uint m_iNorDeadPlus;

    _bool m_isPinky = FALSE;

    _float m_fMoveAngle = 0.f;
protected:
    virtual void Free();

};

