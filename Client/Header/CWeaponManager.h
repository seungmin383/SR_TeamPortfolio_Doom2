#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CWeapon.h"

class CWeaponManager :  public CBase
{
	DECLARE_SINGLETON(CWeaponManager)

public:
	enum class WEAPON_ID {PISTOL, SHOTGUN, CHAINGUN, ROCKET, BFG, ID_END};

private:
	explicit CWeaponManager();
	virtual ~CWeaponManager();

public:
	void Update_Weapon(const _float& fTimeDelta);

public:
	CWeapon* Get_Weapon(WEAPON_ID eID) { return m_mapWeapon[eID]; }

	CWeapon* Get_CurWeapon() { return m_pCurWeapon; }
	WEAPON_ID Get_CurWeaponID() { return m_eCurID; }

	CWeapon* Get_PrevWeapon() { return m_pPrevWeapon; }
	WEAPON_ID Get_PrevWeaponID() { return m_ePrevID; }

public:
	void Add_Weapon(WEAPON_ID eID, CWeapon* pWeapon);
	void Change_Weapon(WEAPON_ID eID);
	void Add_Ammo(CWeapon::AMMO_TYPE eAmmoType, _int iCnt);
	_bool Consume_Ammo(CWeapon::AMMO_TYPE eAmmoType);
	void Fire_Weapon();

private:
	map<WEAPON_ID, CWeapon*> m_mapWeapon;
	map<CWeapon::AMMO_TYPE, _int> m_mapAmmo;
	map<CWeapon::AMMO_TYPE, _int> m_mapMaxAmmo;

	CWeapon* m_pCurWeapon;
	CWeapon* m_pPrevWeapon;

	WEAPON_ID m_eCurID;
	WEAPON_ID m_ePrevID;

private:
	virtual void Free();
};