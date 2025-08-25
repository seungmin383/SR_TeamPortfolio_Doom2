#include "pch.h"
#include "CChaingun.h"
#include "CWeaponManager.h"
#include "CManagement.h"

CChaingun::CChaingun()
{
}

CChaingun::~CChaingun()
{
}

HRESULT CChaingun::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::CHAINGUN;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::CLIP;
	//m_iDamage = 10;
	m_fFireDelay = 0.114f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CChaingun::Fire()
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
		CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::PLAYER_NORMAL_BULLET, OBJID::OBJ_BULLET, vPlayerPos, pPlayer, tmp);

		return true;
	}

	return false;
}

CChaingun* CChaingun::Create()
{
	CChaingun* pChaingun = new CChaingun();

	if (FAILED(pChaingun->Ready_GameObject()))
	{
		Safe_Release(pChaingun);
		MSG_BOX("Chaingun Create Failed");
		return nullptr;
	}

	return pChaingun;
}


void CChaingun::Free()
{
}