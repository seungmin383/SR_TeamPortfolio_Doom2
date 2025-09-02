#include "pch.h"
#include "CShotgun.h"
#include "CWeaponManager.h"
#include "CShotgunBullet.h"
#include "CManagement.h"
#include "CDynamicCamera.h"
#include "CSoundManager.h"

CShotgun::CShotgun()
{
}

CShotgun::~CShotgun()
{
}

HRESULT CShotgun::Ready_GameObject()
{
	m_eWeaponID = WEAPON_ID::SHOTGUN;
	m_eCurState = WEAPON_STATE::IDLE;
	m_eAmmoType = AMMO_TYPE::SHELL;
	m_fFireDelay = 0.8f;
	m_fElapsedTime = 0.f;

	return S_OK;
}

_bool CShotgun::Fire()
{
	if (m_eCurState != WEAPON_STATE::IDLE || m_fElapsedTime > 0.f)
		return false;

	if (CWeaponManager::GetInstance()->Consume_Ammo(m_eAmmoType))
	{
		m_eCurState = WEAPON_STATE::SHOT;
		m_fElapsedTime = m_fFireDelay;

		CGameObject* pPlayer = CManagement::GetInstance()->Get_Player();
		_vec3 vPlayerPos, vAt, vLook;
		pPlayer->Get_Transform()->Get_Pos(&vPlayerPos);
		vAt = static_cast<CDynamicCamera*>(pPlayer)->Get_At();
		vLook = vAt - vPlayerPos;
		D3DXVec3Normalize(&vLook, &vLook);

		CSoundManager::GetInstance()->PlaySoundW(L"DSSHOTGN.wav", CHANNELID::SOUND_EFFECT, 0.5f);
		CGameObject* tmp = nullptr;

		for (int i = 0; i < 5; i++)
		{
			// 샷건 투사체 발사
			CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::PLAYER_SHOTGUN_BULLET, OBJID::OBJ_BULLET, vPlayerPos, pPlayer, tmp);

			if (tmp == nullptr)
				return false;

			tmp->Set_Dead(false);
			tmp->Get_Transform()->Set_Pos(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

			_float fAngle = 0.f;

			switch (i)
			{
			case 0:
				fAngle = -0.03f;
				break;
			case 1:
				fAngle = -0.015f;
				break;
			case 2:
				fAngle = -0.f;
				break;
			case 3:
				fAngle = 0.015f;
				break;
			case 4:
				fAngle = 0.03f;
				break;
			}

			_vec3 tmpAngle = vLook;
			_matrix rotMat;
			D3DXMatrixIdentity(&rotMat);
			D3DXMatrixRotationY(&rotMat, fAngle);
			D3DXVec3TransformNormal(&tmpAngle, &tmpAngle, &rotMat);

			dynamic_cast<CBullet*>(tmp)->Set_InitDir(tmpAngle);
		}

		return true;
	}
	
	return false;
}

CShotgun* CShotgun::Create()
{
	CShotgun* pShotgun = new CShotgun();

	if (FAILED(pShotgun->Ready_GameObject()))
	{
		Safe_Release(pShotgun);
		MSG_BOX("Shotgun Create Failed");
		return nullptr;
	}

	return pShotgun;
}


void CShotgun::Free()
{
}