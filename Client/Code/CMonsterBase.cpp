#include "pch.h"
#include "CMonsterBase.h"

CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
{
}

CMonsterBase::CMonsterBase(const CCharacter& rhs)
	:CCharacter(rhs)
{
}

CMonsterBase::~CMonsterBase()
{
}

void CMonsterBase::Free()
{
	Engine::CCharacter::Free();
}