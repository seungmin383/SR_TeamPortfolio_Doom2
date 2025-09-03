#include "pch.h"
#include "CBfg.h"
#include "CWeaponManager.h"
#include "CBfgBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CSoundManager.h"

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
	m_fFireDelay = 1.5f;
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

		// 투사체 위치 조정하는 부분
		CGameObject* pPlayer = CManagement::GetInstance()->Get_Player();
		_vec3 vPlayerPos;
		_vec3 vPlayerDir, vPlayerAt;

		vPlayerPos = static_cast<CDynamicCamera*>(pPlayer)->Get_Pos();
		vPlayerAt = static_cast<CDynamicCamera*>(pPlayer)->Get_At();
		vPlayerDir = vPlayerAt - vPlayerPos;
		D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

		vPlayerPos += vPlayerDir * 5.f;

		CGameObject* tmp = nullptr;
		// 로켓 투사체 발사
		CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::PLAYER_BFG_BULLET, OBJID::OBJ_BULLET, vPlayerPos, pPlayer, tmp);
		CSoundManager::GetInstance()->PlaySoundW(L"WPBFG.wav", CHANNELID::SOUND_EFFECT, 0.5f);

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