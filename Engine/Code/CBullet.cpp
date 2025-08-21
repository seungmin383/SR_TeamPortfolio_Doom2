#include "CBullet.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CBullet::CBullet(const CBullet& rhs)
	:CGameObject(rhs)
{
}

CBullet::~CBullet()
{
}

void CBullet::OnCollision(CCollider* _pOther)
{
}

void CBullet::OnCollisionEnter(CCollider* _pOther)
{
}

void CBullet::OnCollisionExit(CCollider* _pOther)
{
}

CGameObject* CBullet::Clone()
{
	return nullptr;
}

void CBullet::Free()
{
}
