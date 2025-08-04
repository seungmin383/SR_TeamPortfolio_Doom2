#pragma once
#include "CMonsterBase.h"

class CZombieMan :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private :
    explicit CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CZombieMan(const CGameObject& rhs);
    virtual ~CZombieMan();



};

