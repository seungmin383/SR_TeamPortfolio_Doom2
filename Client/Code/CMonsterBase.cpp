#include "CMonsterBase.h"

CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CMonsterBase::CMonsterBase(const CGameObject& rhs)
	:CGameObject(rhs)
{
}

CMonsterBase::~CMonsterBase()
{
	Engine::CGameObject::Free();
}
