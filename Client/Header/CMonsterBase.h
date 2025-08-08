#pragma once
#include "CCharacter.h"
#include "CPrototypeManager.h"

#define ANIM_MOVE 0
#define ANIM_ATK 1
#define ANIM_NDEAD 2
#define ANIM_GDEAD 3

class CMonsterBase :public CCharacter
{
    //몬스터 자료형을 통일시키고 싶지만 GameObject로 통일하고 싶진 않아서 만든 클래스 
protected:
    explicit CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonsterBase(const CMonsterBase& rhs);
    virtual ~CMonsterBase();

public:
    virtual void Thinker(const _float& fTimeDelta)PURE;                            //몬스터 Ai 관리할듯                
    virtual void Manage_MoveState()PURE;        // 몬스터의 이동 방향을 결정할 함수
    virtual void Change_Animation()PURE;         // 플레이어와의 각도와 몬스터의 이동 방향을 계산해서 실제 선택될 애니메이션 상태를 확인하는 함수
    virtual void Attack_Player(const _float& fTimeDelta)PURE;

    virtual void Debug_Triangle(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2);
    virtual void Debug_Rectangle(_vec3 _LeftTop, _vec3 _RightTop , _vec3 _RightBottom, _vec3 _LeftBottom);
    virtual _vec3 Set_Move_Direction(_vec3* _Look);

    //몬스터 룩(다른 룩을 넣어줘야됨)이랑 몬스터 위치
    virtual _float Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos);
    virtual _int Calc_AnimState(_vec3 vLook, _vec3 vPos);

    //Thinker 개편
    virtual void Thinker1(const _float& fTimeDelta);                    //생각하는 함수 -> 몬스터 AI
    virtual void Perception();                                                         //인지하는 함수 -> 몬스터가 어떤 행동을 할지 상태를 지정해주는 함수
        virtual _bool Check_Target();                                               //시야에 플레이어가 있는지 확인하는 함수

    virtual void Decision();                                                            //의사 결정하는 함수
    virtual void Action(const _float& fTimeDelta);                        //행동하는 함수
  
    //기능 Function (기능 기능?)
   virtual  void        Set_OnTerrain();

   //Setting Function
   virtual HRESULT     Add_Component();

protected:
    Engine::CCalculator*            m_pCalculatorCom;
    Engine::CRectSpriteTex*      m_pBufferCom;
    Engine::CTexture*                m_pTextureCom;
    Engine::CAnimator*             m_pAnimCom;

protected:
    int m_eCurAnimState;
    int m_ePreAnimState;

    _vec3 m_vMoveLook;
    _vec3 m_vtmpLook;

    _float m_fSightAngle;
    _float m_fSightDistance;

    _float m_fAtkDelay; 
    _float m_fMoveDelay;

    _bool m_isMove;
    _bool m_isAtk;


    //Thinker Variable
    _bool m_isDetect_Player;                //시야에 플레이어가 들어왔는가?
    _bool m_isHit;                                 //총알에 맞았는지?
    _bool m_isDead;                             //죽었는가?
    _bool m_isNormalDead;                  //그냥 죽었는가?
    _bool m_isGibbingDead;                 //폭사했는가?

protected:
    virtual void Free();

};

