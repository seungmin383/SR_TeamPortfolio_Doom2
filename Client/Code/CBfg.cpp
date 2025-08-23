#include "pch.h"
#include "CBfg.h"
#include "CWeaponManager.h"

CBfg::CBfg()
{
}

CBfg::~CBfg()
{
}

HRESULT CBfg::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::BFG;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::CELL;
	m_iDamage = 800;
	m_fFireDelay = 1.14f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CBfg::Fire()
{
	if (m_eCurState != WEAPON_STATE::IDLE || m_fElapsedTime > 0.f)
		return false;

	if (CWeaponManager::GetInstance()->Consume_Ammo(m_eAmmoType))
	{
		m_eCurState = WEAPON_STATE::SHOT;
		m_fElapsedTime = m_fFireDelay;
		return true;
	}

	return false;
}

CBfg* CBfg::Create()
{
	CBfg* pBfg = new CBfg();

	if (FAILED(pBfg->Ready_GameObject()))
	{
		Safe_Release(pBfg);
		MSG_BOX("Bfg Create Failed");
		return nullptr;
	}

	return pBfg;
}


void CBfg::Free()
{
}