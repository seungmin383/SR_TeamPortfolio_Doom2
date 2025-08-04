#pragma once
#include "CGameObject.h"

class CMonsterBase :public CGameObject
{
    //몬스터 자료형을 통일시키고 싶지만 GameObject로 통일하고 싶진 않아서 만든 클래스 
protected:
    explicit CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMonsterBase(const CGameObject& rhs);
    virtual ~CMonsterBase();

};

