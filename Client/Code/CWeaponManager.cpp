#include "pch.h"
#include "CWeaponManager.h"

IMPLEMENT_SINGLETON(CWeaponManager)

CWeaponManager::CWeaponManager() :
	m_pCurWeapon(nullptr), m_pPrevWeapon(nullptr),
	m_eNextWeaponID(WEAPON_ID::ID_END),
	m_bIsSwapping(false),
	m_fSwapTimer(0.f)	
{
	m_mapMaxAmmo.emplace(AMMO_TYPE::CLIP, 400);
	m_mapMaxAmmo.emplace(AMMO_TYPE::SHELL, 100);
	m_mapMaxAmmo.emplace(AMMO_TYPE::ROCKET, 100);
	m_mapMaxAmmo.emplace(AMMO_TYPE::CELL, 600);

	m_mapAmmo.emplace(AMMO_TYPE::CLIP, 50);
	m_mapAmmo.emplace(AMMO_TYPE::SHELL, 50);
}

CWeaponManager::~CWeaponManager()
{
	Free();
}

void CWeaponManager::Update_Weapon(const _float& fTimeDelta)
{
	if (m_pCurWeapon != nullptr)
	{
		m_pCurWeapon->Update_GameObject(fTimeDelta);

		// 무기 교체 관련 로직
		if (m_pCurWeapon->Get_WeaponState() != WEAPON_STATE::SWAP)
			return;

		m_fSwapTimer += fTimeDelta;

		// 0.5초가 지나고, 교체가 안된 상태라면 무기 교체
		if (m_fSwapTimer >= 0.5f && m_pCurWeapon->Get_WeaponID() != m_eNextWeaponID)
		{
			m_pPrevWeapon = m_pCurWeapon;
			m_pCurWeapon = m_mapWeapon[m_eNextWeaponID];
		}
		else if (m_fSwapTimer >= 1.f)
		{
			m_pCurWeapon->Set_WeaponState(WEAPON_STATE::IDLE);
			m_pPrevWeapon->Set_WeaponState(WEAPON_STATE::IDLE);
		}
	}
}

void CWeaponManager::Add_Weapon(WEAPON_ID eID, CWeapon* pWeapon)
{
	// 추가하려는 무기가 있다면 리턴
	auto iter = m_mapWeapon.find(eID);
	if (iter != m_mapWeapon.end())
		return;

	m_mapWeapon.emplace(eID, pWeapon);
}

void CWeaponManager::Change_Weapon(WEAPON_ID eID)
{
	// 찾으려는 무기가 없다면 리턴
	auto iter = m_mapWeapon.find(eID);
	if (iter == m_mapWeapon.end())
		return;

	// 바꾸려는 무기가 현재 무기라면 리턴
	if (iter->second == m_pCurWeapon)
		return;

	// 무기의 상태가 사격, 교체 상태면 리턴
	if (iter->second->Get_WeaponState() == WEAPON_STATE::SHOT ||
		iter->second->Get_WeaponState() == WEAPON_STATE::SWAP)
		return;

	// 무기를 교체 상태로 변경 후 교체 예정인 ID를 저장
	m_pCurWeapon->Set_WeaponState(WEAPON_STATE::SWAP);
	iter->second->Set_WeaponState(WEAPON_STATE::SWAP);
	m_fSwapTimer = 0.f;
	m_eNextWeaponID = eID;
}

void CWeaponManager::Add_Ammo(AMMO_TYPE eAmmoType, _int iCnt)
{
	// 총알의 최대 개수를 초과하면 최대 개수로 고정
	int iMaxAmmo = m_mapMaxAmmo[eAmmoType];

	m_mapAmmo[eAmmoType] += iCnt;

	if (m_mapAmmo[eAmmoType] >= iMaxAmmo)
		m_mapAmmo[eAmmoType] = iMaxAmmo;
}

_bool CWeaponManager::Consume_Ammo(AMMO_TYPE eAmmoType)
{
	// 총알의 개수가 0보다 작다면
	if (m_mapAmmo[eAmmoType] < 0)
	{
		m_mapAmmo[eAmmoType] = 0;
		return false;
	}
	
	m_mapAmmo[eAmmoType]--;
	
	return true;
}

bool CWeaponManager::Fire_Weapon()
{
	if (m_pCurWeapon != nullptr)
	{
		// 총알이 없다면 이전 무기로 변경
		AMMO_TYPE eType = m_pCurWeapon->Get_AmmoType();
		if (m_mapAmmo[eType] < 0)
		{
			Change_Weapon(m_pPrevWeapon->Get_WeaponID());
			return false;
		}

		m_pCurWeapon->Set_WeaponState(WEAPON_STATE::IDLE);

		m_pCurWeapon->Fire();

		return true;
	}
}

void CWeaponManager::Free()
{
	for_each(m_mapWeapon.begin(), m_mapWeapon.end(), CDeleteMap());
}
