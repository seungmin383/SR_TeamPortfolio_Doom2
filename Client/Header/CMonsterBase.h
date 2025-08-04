#pragma once
#include "CCharacter.h"

class CMonsterBase :public CCharacter
{
    //몬스터 자료형을 통일시키고 싶지만 GameObject로 통일하고 싶진 않아서 만든 클래스 
protected:
    explicit CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonsterBase(const CCharacter& rhs);
    virtual ~CMonsterBase();

public:
    virtual void Thinker()PURE;                            //몬스터 Ai 관리할듯                
    virtual void Manage_MoveState()PURE;        // 몬스터의 이동 방향을 결정할 함수
    virtual void Manage_AnimState()PURE;        // 플레이어와의 각도와 몬스터의 이동 방향을 계산해서 실제 선택될 애니메이션 상태를 확인하는 함수

protected:
    virtual void Free();

};

