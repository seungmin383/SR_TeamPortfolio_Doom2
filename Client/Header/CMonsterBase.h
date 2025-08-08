#pragma once
#include "CCharacter.h"

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
    virtual void Check_Target()PURE;                  //플레이어 찾는 함수
    virtual void Attack_Player(const _float& fTimeDelta)PURE;

    virtual void Debug_Triangle(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2);
    virtual void Debug_Rectangle(_vec3 _LeftTop, _vec3 _RightTop , _vec3 _RightBottom, _vec3 _LeftBottom);
    virtual _vec3 Set_Direction(_vec3* _Look);

    //몬스터 룩(다른 룩을 넣어줘야됨)이랑 몬스터 위치
    virtual _float Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos);
    virtual _int Calc_AnimState(_vec3 vLook, _vec3 vPos);

protected:
    // 빌보드 적용하니까 이동을 어떻게 해야될지 모르겠어서 새로 만듬
    Engine::CTransform* m_pMoveTransCom;

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
    _bool m_isNormalDead;
    _bool m_isGibbingDead;

protected:
    virtual void Free();

};

