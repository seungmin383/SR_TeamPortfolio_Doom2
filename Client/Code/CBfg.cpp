#include "pch.h"
#include "CBfg.h"
#include "CWeaponManager.h"
#include "CBfgBullet.h"
#include "CManagement.h"

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

		CGameObject* pPlayer = CManagement::GetInstance()->Get_Player();
		_vec3 vPlayerPos;
		pPlayer->Get_Transform()->Get_Pos(&vPlayerPos);

		CGameObject* tmp = nullptr;
		// 로켓 투사체 발사
		CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::PLAYER_BFG_BULLET, OBJID::OBJ_BULLET, vPlayerPos, pPlayer, tmp);

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