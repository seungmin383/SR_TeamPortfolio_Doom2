#include "pch.h"
#include "CImp.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CBullet.h"
#include "CSoundManager.h"


CImp::CImp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CImp::CImp(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CImp::~CImp()
{
}

HRESULT CImp::Ready_GameObject()
{

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(52.f, 0.f, 10.f);

	_vec3 vPos;
	_vec3 vScale = { 2.f, 2.f, 2.f };

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(2.f, 3.f, 2.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_ENEMY,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COL_BULLET | COLLAYERID::COL_ENEMY);

#pragma region ANIM_SETTING

	//이동
	m_pAnimCom->Add_Animation(
		MOVE,
		m_pTextureCom,
		m_pBufferCom,
		4,				//가로
		1,				//세로
		0,				//start frame
		3,				//end frame
		0.2f,
		true,
		false
	);

	//공격
	m_pAnimCom->Add_Animation(
		ATK,
		m_pTextureCom,
		m_pBufferCom,
		3,
		1,
		0,
		2,
		0.2f,
		true,
		false
	);

	//일반 죽음
	m_pAnimCom->Add_Animation(
		NOR_DEATH,
		m_pTextureCom,
		m_pBufferCom,
		6,
		1,
		0,
		5,
		0.15f,
		false,
		false
	);

	//폭사
	m_pAnimCom->Add_Animation(
		GIB_DEATH,
		m_pTextureCom,
		m_pBufferCom,
		8,
		1,
		0,
		7,
		0.15f,
		false,
		false
	);

	//피격
	m_pAnimCom->Add_Animation(
		HIT,
		m_pTextureCom,
		m_pBufferCom,
		1,
		1,
		0,
		1,
		0.2f,
		false,
		false
	);
#pragma endregion

#pragma region VARIABLE_SETTING
	m_fSightAngle = D3DXToRadian(80.f);
	m_fSightDistance = 50.f;

	m_fAtkSpeed = rand() % 4 + 2;
	m_iGameHp = 60;
	m_iAtkDmg = 3;

	m_fSpeed = 2.f;
	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

#pragma endregion

	CMonsterBase::Ready_GameObject();

	Set_OnTerrain();
	CGameObject::LateUpdate_GameObject(CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));

	return S_OK;
}

_int CImp::Update_GameObject(const _float& fTimeDelta)
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	LONG minx = min(pos.x + 5, pos.x - 5);
	LONG maxx = max(pos.x + 5, pos.x - 5);
	LONG minz = min(pos.z + 5, pos.z - 5);
	LONG maxz = max(pos.z + 5, pos.z - 5);

	RECT myRect{ minx, minz, maxx, maxz };
	if (CCollisionManager::Check_AABB(myRect) == false)
		return 0;

	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();

	if (m_bDead)
		return Calc_Death_Time(fTimeDelta);

	return 0;
}

void CImp::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	LONG minx = min(pos.x + 5, pos.x - 5);
	LONG maxx = max(pos.x + 5, pos.x - 5);
	LONG minz = min(pos.z + 5, pos.z - 5);
	LONG maxz = max(pos.z + 5, pos.z - 5);

	RECT myRect{ minx, minz, maxx, maxz };
	if (CCollisionManager::Check_AABB(myRect) == false)
		return;

	CMonsterBase::LateUpdate_GameObject(fTimeDelta);
}

void CImp::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
}

_bool CImp::Check_Attack(const _float& fTimeDelta)
{
	m_fAtkDelay += fTimeDelta;

	if (m_fAtkDelay > m_fAtkSpeed)
	{
		if (m_AtkEnterCnt == 0)									//이 함수에 처음 입장했다면
		{
			m_AtkEnterCnt++;										//카운트만 올리고 return 
			return TRUE;
		}
		else
		{
			if (m_pAnimCom->Get_isFinAct())			//애니메이션이 끝났다면
			{
				m_AtkEnterCnt = 0;
				m_fAtkDelay = 0.f;
				m_isDetected_Player = FALSE;
				m_isShoot = FALSE;
				return FALSE;
			}
			else
			{
				m_AtkEnterCnt++;
				return TRUE;
			}
		}
	}
	else
		return FALSE;
}

void CImp::Play_Sound()
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	switch (m_iCurThought)
	{
	case AI_DEAD:
		if(MONSTER_STATE::NOR_DEATH)
			CSoundManager::GetInstance()->PlaySoundW(L"IMPDEATH1.wav", CHANNELID::SOUND_EFFECT, 1.f);

		if(MONSTER_STATE::GIB_DEATH)
			CSoundManager::GetInstance()->PlaySoundW(L"IMPDEATH2.wav", CHANNELID::SOUND_EFFECT, 1.f);
		break;

	case AI_MOVE:
		CSoundManager::GetInstance()->PlaySoundW(L"IMPIDLE.wav", CHANNELID::SOUND_EFFECT, 1.f);
		break;

	case AI_CHASE:
		CSoundManager::GetInstance()->PlaySoundW(L"IMPFINDPLAYER1.wav", CHANNELID::SOUND_EFFECT, 1.f);
		break;

	case AI_ATTACK:
		CSoundManager::GetInstance()->PlaySoundW(L"IMPFINDPLAYER2.wav", CHANNELID::SOUND_EFFECT, 1.f);
		break;

	case AI_HIT:
		CSoundManager::GetInstance()->PlaySound3D(L"DEMONHIT", vPos, 1.f);
		break;
	}

	m_iPreThought = m_iCurThought;
}

void CImp::Attack(const _float fTimeDelta)
{
	m_isShoot = TRUE;
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	CGameObject* tmp = nullptr;
	vPos.y += 1;

	if (FAILED(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_IMP_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp)))
	{
		int a = 0;
	}
	if (tmp == nullptr)
		return;

	tmp->Set_Dead(FALSE);
	tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
	dynamic_cast<CBullet*>(tmp)->Set_InitDir(m_vMoveLook);
}

void CImp::Drop_Item()
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ITEM_CLIP, OBJID::OBJ_ITEM, vPos, 1);
}

HRESULT CImp::Add_Component()
{
	CMonsterBase::Add_Component();

	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Imp_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CImp* CImp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CImp* tmp = new CImp(pGraphicDev);

	return tmp;
}

CImp* CImp::Clone()
{
	CImp* PImp = new CImp(*this);

	if (FAILED(PImp->Ready_GameObject()))
	{
		Safe_Release(PImp);
		MSG_BOX("ZombieMan Create Failed");
		return nullptr;
	}

	return PImp;
}

void CImp::Free()
{
	Engine::CGameObject::Free();
}

