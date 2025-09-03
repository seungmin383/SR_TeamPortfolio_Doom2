#include "pch.h"
#include "CRocket.h"
#include "CWeaponManager.h"
#include "CRocketBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CSoundManager.h"

CRocket::CRocket()
{
}

CRocket::~CRocket()
{
}

HRESULT CRocket::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::ROCKET;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::ROCKET;
	m_iDamage = 160;
	m_fFireDelay = 1.06f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CRocket::Fire()
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

		vPlayerPos =static_cast<CDynamicCamera*>(pPlayer)->Get_Pos();
		vPlayerAt = static_cast<CDynamicCamera*>(pPlayer)->Get_At();
		vPlayerDir = vPlayerAt - vPlayerPos;
		D3DXVec3Normalize(&vPlayerDir, &vPlayerDir);

		vPlayerPos += vPlayerDir * 5.f;

		CGameObject* tmp = nullptr;
		// 로켓 투사체 발사
		CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::PLAYER_ROCKET_BULLET, OBJID::OBJ_BULLET, vPlayerPos, pPlayer, tmp);
		CSoundManager::GetInstance()->PlaySoundW(L"WPROCKET.wav", CHANNELID::SOUND_EFFECT, 0.5f);

		return true;
	}

	return false;
}

CRocket* CRocket::Create()
{
	CRocket* pRocket = new CRocket();

	if (FAILED(pRocket->Ready_GameObject()))
	{
		Safe_Release(pRocket);
		MSG_BOX("Rocket Create Failed");
		return nullptr;
	}

	return pRocket;
}


void CRocket::Free()
{
}