#include "pch.h"
#include "CWeaponManager.h"

IMPLEMENT_SINGLETON(CWeaponManager)

CWeaponManager::CWeaponManager() :
	m_pCurWeapon(nullptr), m_pPrevWeapon(nullptr),
	m_eCurID(WEAPON_ID::ID_END), m_ePrevID(WEAPON_ID::ID_END)
{
	m_mapMaxAmmo.emplace(CWeapon::AMMO_TYPE::CLIP, 400);
	m_mapMaxAmmo.emplace(CWeapon::AMMO_TYPE::SHELL, 100);
	m_mapMaxAmmo.emplace(CWeapon::AMMO_TYPE::ROCKET, 100);
	m_mapMaxAmmo.emplace(CWeapon::AMMO_TYPE::CELL, 600);

	m_mapAmmo.emplace(CWeapon::AMMO_TYPE::CLIP, 50);
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

	// 이전 무기에 현재 무기를 저장 후 무기 교체
	m_pPrevWeapon = m_pCurWeapon;
	m_ePrevID = m_eCurID;
	m_pCurWeapon = iter->second;
	m_eCurID = eID;
	//m_pCurWeapon->Set_WeaponState(CWeapon::WEAPON_STATE::IDLE);
}

void CWeaponManager::Add_Ammo(CWeapon::AMMO_TYPE eAmmoType, _int iCnt)
{
	// 총알의 최대 개수를 초과하면 리턴
	int iMaxAmmo = m_mapMaxAmmo[eAmmoType];

	m_mapAmmo[eAmmoType] += iCnt;

	if (m_mapAmmo[eAmmoType] >= iMaxAmmo)
		m_mapAmmo[eAmmoType] = iMaxAmmo;
}

_bool CWeaponManager::Consume_Ammo(CWeapon::AMMO_TYPE eAmmoType)
{
	// 총알의 개수가 0보다 작다면
	if (m_mapAmmo[eAmmoType] <= 0)
		return false;
	
	m_mapAmmo[eAmmoType]--;
	
	return true;
}

void CWeaponManager::Fire_Weapon()
{
	if (m_pCurWeapon != nullptr)
	{
		m_pCurWeapon->Set_WeaponState(CWeapon::WEAPON_STATE::IDLE);
		m_pCurWeapon->Fire();
	}
}

void CWeaponManager::Free()
{
	for_each(m_mapWeapon.begin(), m_mapWeapon.end(), CDeleteMap());
}
