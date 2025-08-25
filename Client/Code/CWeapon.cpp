#include "pch.h"
#include "CWeapon.h"

CWeapon::CWeapon() :
	m_eCurState(WEAPON_STATE::IDLE), m_eAmmoType(AMMO_TYPE::AMMO_END),
	m_iDamage(0),
	m_fFireDelay(0.f), m_fElapsedTime(0.f)
{
}

CWeapon::~CWeapon()
{
}

_int CWeapon::Update_GameObject(const _float& fTimeDelta)
{
	if (m_fElapsedTime >= 0.f)
		m_fElapsedTime -= fTimeDelta;

	if (m_eCurState == WEAPON_STATE::SHOT && m_fElapsedTime < 0.f)
		m_eCurState = WEAPON_STATE::IDLE;

	return 0;
}


void CWeapon::Free()
{
}
