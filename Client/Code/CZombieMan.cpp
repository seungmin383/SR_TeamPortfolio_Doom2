#include "pch.h"
#include "CZombieMan.h"

CZombieMan::CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonsterBase(pGraphicDev)
{
}

CZombieMan::CZombieMan(const CGameObject& rhs)
	:CMonsterBase(rhs)
{
}

CZombieMan::~CZombieMan()
{
	CMonsterBase::Free();
}
