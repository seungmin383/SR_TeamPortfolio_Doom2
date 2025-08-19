#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CWeapon.h"

class CWeaponManager :  public CBase
{
	DECLARE_SINGLETON(CWeaponManager)

private:
	explicit CWeaponManager();
	virtual ~CWeaponManager();

public:
	void Update_Weapon(const _float& fTimeDelta);

public:
	CWeapon* Get_Weapon(WEAPON_ID eID) { return m_mapWeapon[eID]; }		// 특정 무기 가져오는 함수

	CWeapon* Get_CurWeapon() { return m_pCurWeapon; }						// 현재 무기 가져오는 함수
	void Set_CurWeapon(WEAPON_ID eID) {	m_pCurWeapon = m_mapWeapon[eID]; }	// 현재 무기 설정하는 함수

	CWeapon* Get_PrevWeapon() { return m_pPrevWeapon; }					// 이전 무기 가져오는 함수

	_float	Get_SwapTime() { return m_fSwapTimer; }

	_int	Get_Ammo(AMMO_TYPE eType) { return m_mapAmmo[eType]; }

public:
	void Add_Weapon(WEAPON_ID eID, CWeapon* pWeapon);					// 무기 추가 함수
	void Change_Weapon(WEAPON_ID eID);									// 무기 교체 함수
	void Add_Ammo(AMMO_TYPE eAmmoType, _int iCnt);						// 총알 추가 함수
	_bool Consume_Ammo(AMMO_TYPE eAmmoType);							// 총알 소모 함수
	_bool Fire_Weapon();												// 무기 발사 함수

private:
	map<WEAPON_ID, CWeapon*> m_mapWeapon;			// 무기 담긴 컨테이너
	map<AMMO_TYPE, _int> m_mapAmmo;		// 총알 담긴 컨테이너
	map<AMMO_TYPE, _int> m_mapMaxAmmo;		// 무기별 최대 총알 개수 담긴 컨테이너

	CWeapon* m_pCurWeapon;		// 현재 무기
	CWeapon* m_pPrevWeapon;		// 이전 무기

	WEAPON_ID	m_eNextWeaponID;
	_bool	m_bIsSwapping;

	_float	m_fSwapTimer;

private:
	virtual void Free();
};