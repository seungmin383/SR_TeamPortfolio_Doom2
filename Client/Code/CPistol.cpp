#include "pch.h"
#include "CPistol.h"
#include "CWeaponManager.h"

CPistol::CPistol()
{
}

CPistol::~CPistol()
{
}

HRESULT CPistol::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::PISTOL;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::CLIP;
	m_iDamage = 10;
	m_fFireDelay = 0.5f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CPistol::Fire()
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

CPistol* CPistol::Create()
{
	CPistol* pPistol = new CPistol();

	if (FAILED(pPistol->Ready_GameObject()))
	{
		Safe_Release(pPistol);
		MSG_BOX("Pistol Create Failed");
		return nullptr;
	}

	return pPistol;
}


void CPistol::Free()
{
}