#include "pch.h"
#include "CShotgunGuy.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CBullet.h"
#include "CSoundManager.h"

CShotgunGuy::CShotgunGuy(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CShotgunGuy::CShotgunGuy(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CShotgunGuy::~CShotgunGuy()
{
}

HRESULT CShotgunGuy::Ready_GameObject()
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
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COLLAYERID::COL_ENEMY);


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
		0.1f,
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
		10,
		1,
		0,
		9,
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
	m_iGameHp = 30;
	m_iAtkDmg = 3;

	m_fSpeed = 2.f;
	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
#pragma endregion

	CMonsterBase::Ready_GameObject();

	//CMonsterBase::Update_GameObject(CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));
	//Bill_Board();
	//Set_OnTerrain();
	//CGameObject::LateUpdate_GameObject(CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));

	return S_OK;
}

_int CShotgunGuy::Update_GameObject(const _float& fTimeDelta)
{
	//_vec3 pos;
	//m_pTransformCom->Get_Pos(&pos);

	//LONG minx = min(pos.x + 5, pos.x - 5);
	//LONG maxx = max(pos.x + 5, pos.x - 5);
	//LONG minz = min(pos.z + 5, pos.z - 5);
	//LONG maxz = max(pos.z + 5, pos.z - 5);

	//RECT myRect{ minx, minz, maxx, maxz };
	//if (CCollisionManager::Check_AABB(myRect) == false)
	//	return 0;

	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();

	if (m_bDead)
		return Calc_Death_Time(fTimeDelta);

	return 0;
}

void CShotgunGuy::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//_vec3 pos;
	//m_pTransformCom->Get_Pos(&pos);

	//LONG minx = min(pos.x + 5, pos.x - 5);
	//LONG maxx = max(pos.x + 5, pos.x - 5);
	//LONG minz = min(pos.z + 5, pos.z - 5);
	//LONG maxz = max(pos.z + 5, pos.z - 5);

	//RECT myRect{ minx, minz, maxx, maxz };
	//if (CCollisionManager::Check_AABB(myRect) == false)
	//	return;

	CMonsterBase::LateUpdate_GameObject(fTimeDelta);
}

void CShotgunGuy::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
}

void CShotgunGuy::Attack(const _float fTimeDelta)
{
	m_isShoot = TRUE;
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	CGameObject* tmp = nullptr;

	for (int i = 0; i < 5; i++)
	{
		if (FAILED(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_ZOMBIEMAN_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp)))
		{
			int a = 0;
		}

		if (tmp == nullptr)
			return;

		tmp->Set_Dead(FALSE);
		tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
		_float aa = 0;

		switch (i)
		{
		case 0:
			aa = -0.1f;
			break;
		case 1:
			aa = -0.05f;
			break;
		case 2:
			aa = -0.f;
			break;
		case 3:
			aa = 0.05f;
			break;
		case 4:
			aa = 0.1f;
			break;

		}
		_vec3	 tmpAng = m_vMoveLook;
		_matrix rotmat;
		D3DXMatrixIdentity(&rotmat);
		D3DXMatrixRotationY(&rotmat, aa);
		D3DXVec3TransformNormal(&tmpAng, &tmpAng, &rotmat);

		dynamic_cast<CBullet*>(tmp)->Set_InitDir(tmpAng);
	}
}

void CShotgunGuy::Play_Sound()
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	static _bool isFindPlayer = false;
	_int a = 0;
	switch (m_iCurThought)
	{
		//죽었을 때 
	case AI_DEAD:
		CSoundManager::GetInstance()->PlaySound3D(L"SG_DEATH", vPos, 1.f);
		break;

		//맞았을 때
	case AI_HIT:
		a = rand() % 10;

		if (a > 5)
			CSoundManager::GetInstance()->PlaySound3D(L"ZM_HIT", vPos, 1.f);

		break;

		//공격할 때
	case AI_ATTACK:
		CSoundManager::GetInstance()->PlaySound3D(L"SG_ATTACK", vPos, 1.f);
		break;

		//플레이어를 발견했을 때
	case AI_CHASE:
		if 
		(!isFindPlayer)
	{
		CSoundManager::GetInstance()->PlaySound3D(L"SG_CHASE", vPos, 1.f);	
		isFindPlayer = TRUE;
	}

		break;

		//아이들 상태
	case AI_MOVE:
		CSoundManager::GetInstance()->PlaySound3D(L"ZM_MOVE", vPos, 1.f);
		break;
	}

	m_iPreThought = m_iCurThought;
}

void CShotgunGuy::Drop_Item()
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::WEAPON_SHOTGUN, OBJID::OBJ_ITEM, vPos, 1);
	
}


HRESULT CShotgunGuy::Add_Component()
{
	CMonsterBase::Add_Component();

	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ShotgunGuy_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CShotgunGuy* CShotgunGuy::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShotgunGuy* tmp = new CShotgunGuy(pGraphicDev);

	return tmp;
}

CShotgunGuy* CShotgunGuy::Clone()
{
	CShotgunGuy* PShotgunGuy = new CShotgunGuy(*this);

	if (FAILED(PShotgunGuy->Ready_GameObject()))
	{
		Safe_Release(PShotgunGuy);
		MSG_BOX("ArchVile Create Failed");
		return nullptr;
	}

	return PShotgunGuy;
}

void CShotgunGuy::Free()
{
	Engine::CGameObject::Free();
}

