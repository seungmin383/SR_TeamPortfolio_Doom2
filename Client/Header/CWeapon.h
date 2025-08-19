#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CWeapon : public CBase
{
public:
	enum class AMMO_TYPE
	{
		CLIP,
		SHELL,
		ROCKET,
		CELL,
		AMMO_END
	};

protected:
	explicit CWeapon();
	virtual ~CWeapon();

public:
	virtual HRESULT Ready_GameObject() = 0;
	virtual _int Update_GameObject(const _float& fTimeDelta);

public:
	virtual void	Fire() = 0;

public:
	WEAPON_ID		Get_WeaponID() { return m_eWeaponID; }
	WEAPON_STATE	Get_WeaponState() { return m_eCurState; }
	void			Set_WeaponState(WEAPON_STATE eState) { m_eCurState = eState; }

	AMMO_TYPE		Get_AmmoType() { return m_eAmmoType; }

	_int			Get_WeaponDamage() { return m_iDamage; }

protected:
	WEAPON_ID		m_eWeaponID;
	WEAPON_STATE	m_eCurState;
	AMMO_TYPE		m_eAmmoType;

	_int		m_iDamage;

	_float		m_fFireDelay;		// 연사 속도
	_float		m_fElapsedTime;		// 연사 시간 계산을 위한 누적 시간

private:
	virtual void Free() override;
};