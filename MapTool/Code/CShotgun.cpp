#include "pch.h"
#include "CShotgun.h"
#include "CWeaponManager.h"

CShotgun::CShotgun()
{
}

CShotgun::~CShotgun()
{
}

HRESULT CShotgun::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::SHOTGUN;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::SHELL;
	m_iDamage = 50;
	m_fFireDelay = 1.f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CShotgun::Fire()
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

CShotgun* CShotgun::Create()
{
	CShotgun* pShotgun = new CShotgun();

	if (FAILED(pShotgun->Ready_GameObject()))
	{
		Safe_Release(pShotgun);
		MSG_BOX("Shotgun Create Failed");
		return nullptr;
	}

	return pShotgun;
}


void CShotgun::Free()
{
}