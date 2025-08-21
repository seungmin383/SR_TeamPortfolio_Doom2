#include "pch.h"
#include "CZombieManBullet.h"

CZombieManBullet::CZombieManBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBulletBase(pGraphicDev)
{
}

CZombieManBullet::CZombieManBullet(const CZombieManBullet& rhs)
	:CBulletBase(rhs)
{
}

CZombieManBullet::~CZombieManBullet()
{
}

HRESULT CZombieManBullet::Ready_GameObject(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)
{
	return E_NOTIMPL;
}

_int CZombieManBullet::Update_GameObject(const _float& fTimeDelta)
{

	return 0;
}

void CZombieManBullet::Create(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)
{
}

void CZombieManBullet::Free()
{
	CBulletBase::Free();
}
