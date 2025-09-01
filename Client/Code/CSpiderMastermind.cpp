#include "pch.h"
#include "CSpiderMastermind.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CBullet.h"
#include <random>
#include "CLayer.h"
#include"CTerrainSector.h"
#include "CDynamicCamera.h"

#define SPEED 3.f
CSpiderMastermind::CSpiderMastermind(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CSpiderMastermind::CSpiderMastermind(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CSpiderMastermind::~CSpiderMastermind()
{
}

HRESULT CSpiderMastermind::Ready_GameObject()
{


	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(60.f, 0.f, 10.f);
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(8.f, 5.f, 8.f), _vec3(0.f, -5.f, 0.f),
		COLLAYERID::COL_ENEMY,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT);


#pragma region ANIM_SETTING

	//이동
	m_pAnimCom->Add_Animation(
		MOVE,
		m_pTextureCom,
		m_pBufferCom,
		6,				//가로
		1,				//세로
		0,				//start frame
		5,				//end frame
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
		10,
		1,
		0,
		9,
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
	m_pAnimCom->Add_Animation(
		NANRI,
		m_pTextureCom,
		m_pBufferCom,
		6,				//가로
		1,				//세로
		0,				//start frame
		5,				//end frame
		0.05f,
		true,
		false
	);
#pragma endregion

#pragma region VARIABLE_SETTING
	m_fSightAngle = D3DXToRadian(80.f);
	m_fSightDistance = 50.f;

	m_fAtkSpeed = 4.f;
	m_iGameHp = 3000;
	m_iMaxHp = m_iGameHp;
	m_iAtkDmg = 3;

	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;
	m_fSpeed = SPEED;
	m_isBoss = FALSE;
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<uint16_t> dist(0, 0xFFFF);
	m_iPattern = dist(rng);
	Calc_Pattern();

	for (int i = 0; i < 8; i++)
	{
	}
	_vec3 vSize{ 10.f, 10.f, 10.f };
	m_pTransformCom->Set_Scale(vSize);
	m_iHeight = 10;
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

#pragma endregion

	CMonsterBase::Ready_GameObject();
	return S_OK;
}

_int CSpiderMastermind::Update_GameObject(const _float& fTimeDelta)
{
	Check_Anim_Fin(fTimeDelta);
	Pattern_Timer(fTimeDelta);		//패턴 시간 누적

	if (!m_bDead)
	{
		if (m_isAwake)
		{
			Reset_State();
			Awake_Pattern(m_PattenArr[m_iEnt_Pattern_Number]);
			//Awake_Pattern(3); 
			m_isAwake = FALSE;
		}
		Update_Pattern(fTimeDelta, m_PattenArr[m_iEnt_Pattern_Number]);

	}
	//Update_Pattern(fTimeDelta, 3);

//	cout << "스파이더 마스터마인드 픎레이어 발견 : " << m_isDetect_Player << endl;
	if(!m_isPattern3On)
		CMonsterBase::Update_GameObject(fTimeDelta);
	else
	{
		_vec3 vPos;
		m_pTransformCom->Get_Pos(&vPos);

		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
		CGameObject::Update_GameObject(fTimeDelta);

		m_eCurAnimState = Calc_AnimState_byPlayer(m_vMoveLook, vPos);
		Movement(fTimeDelta);
	}

	Bill_Board();
	Set_OnTerrain();


	//if(m_isPattern3On)
	//	cout << "TRUE" << endl;
	//else
	//	cout << "FALSE" << endl;


	//Update_Pattern(fTimeDelta, 2);

	return 0;
}


void CSpiderMastermind::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::LateUpdate_GameObject(fTimeDelta);	
}

void CSpiderMastermind::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
}

void CSpiderMastermind::Awake_Pattern(_int _Pattern)
{
	switch (_Pattern)
	{
	case 0:
		Reset_Pattern1(0.f);
		break;
	case 1:
		Reset_Pattern2(0.f);
		break;
	case 2:
		Reset_Pattern3(0.f);
		break;
	case 3:
		Reset_Pattern4(0.f);
		break;
	}
	m_isAwake = FALSE;
}

void CSpiderMastermind::Reset_State()
{
	m_fSpeed = SPEED;					//속도를 원래대로 돌리고
	m_iTmp_Anim_Type = MOVE;	//애니메이션을 MOVE로 바꾸고
	m_eAnim_Type = MOVE;
	m_isPattern3On = FALSE;

	m_pColliderCom->Set_Active(true);
}

void CSpiderMastermind::Update_Pattern(const _float& fTimeDelta, _int _Pattern)
{
	switch (_Pattern)
	{
	case 0:
		Pattern1(fTimeDelta);
		break;
	case 1:
		Pattern2(fTimeDelta);
		break;
	case 2:
		Pattern3(fTimeDelta);
		break;
	case 3:
		Pattern4(fTimeDelta);
		break;
	}
}

HRESULT CSpiderMastermind::Add_Component()
{
	CMonsterBase::Add_Component();

	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_SpiderMastermind_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


void CSpiderMastermind::Attack(const _float fTimeDelta)
{
	CMonsterBase::Attack(fTimeDelta);
	Fire(fTimeDelta);

}

void CSpiderMastermind::Calc_Pattern()
{
	for (int i = 0; i < 8; i++)
		m_PattenArr[i] = (m_iPattern >> (i * 2)) & 0x3;
}

void CSpiderMastermind::Fire(const _float fTimeDelta)
{
	m_isShoot = TRUE;
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	CGameObject* tmp = nullptr;
	for(int i = 0; i< 3; i++)
	{
		vPos.z -= i;
		if (FAILED(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_ZOMBIEMAN_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp)))
		{
			int a = 0;
		}

		if (tmp == nullptr)
			return;

		tmp->Set_Dead(FALSE);
		tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
		_vec3 vPlayerPos;
		CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&vPlayerPos);
		_vec3 dir = vPlayerPos - vPos;
		D3DXVec3Normalize(&dir, &dir);

		dynamic_cast<CBullet*>(tmp)->Set_InitDir(dir);
	}

}

void CSpiderMastermind::Reset_Pattern1(const _float fTimeDelta)
{

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	CGameObject* tmp = nullptr;
	

	if (m_iGameHp > m_iMaxHp / 2)
	{
#pragma region BulletFire
		if (FAILED(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_BOSS_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp)))
		{
			int a = 0;
		}
		if (tmp == nullptr)
			return;

		tmp->Set_Dead(FALSE);
		tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);

		_vec3 vPlayerPos;
		CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&vPlayerPos);
		_vec3 dir = vPlayerPos - vPos;
		D3DXVec3Normalize(&dir, &dir);

		dynamic_cast<CBullet*>(tmp)->Set_InitDir(dir);
#pragma endregion
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_BOSS_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp);
		
			if (tmp == nullptr)
				return;

			tmp->Set_Dead(FALSE);
			switch (i)
			{
			case 0:
			{
				tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
			}
				break;

			case 1:
			{
				_vec3 vUp = { 0.f,1.f,0.f };
				_vec3 vRight;
				_vec3 vLook = m_vMoveLook;
				D3DXVec3Normalize(&vLook, &vLook);
				D3DXVec3Cross(&vRight, &vUp, &vLook);
				_vec3 vMuzzlePos = vPos + vRight * 10.0f;
				tmp->Get_Transform()->Set_Pos(vMuzzlePos.x, vMuzzlePos.y, vMuzzlePos.z);
			}
				break;

			case 2:
			{
				_vec3 vUp = { 0.f,1.f,0.f };
				_vec3 vLeft;
				_vec3 vLook = m_vMoveLook;
				D3DXVec3Normalize(&vLook, &vLook);
				D3DXVec3Cross(&vLeft, &vLook, &vUp);
				_vec3 vMuzzlePos = vPos + vLeft * 10.0f;
				tmp->Get_Transform()->Set_Pos(vMuzzlePos.x, vMuzzlePos.y, vMuzzlePos.z);
			}
				break;

			}

			_vec3 vPlayerPos;
			CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&vPlayerPos);
			_vec3 dir = vPlayerPos - vPos;
			D3DXVec3Normalize(&dir, &dir);

			dynamic_cast<CBullet*>(tmp)->Set_InitDir(dir);
		}
	}

}

void CSpiderMastermind::Reset_Pattern2(const _float fTimeDelta)
{
	if (m_iGameHp > m_iMaxHp / 2) //1페이즈
	{
		_vec3 vPos;
		m_pTransformCom->Get_Pos(&vPos);

		_vec3 vUp = { 0.f,1.f,0.f };
		_vec3 vRight;
		_vec3 vLook = m_vMoveLook;
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Cross(&vRight, &vUp, &vLook);
		_vec3 vMuzzlePos = vPos + vRight * 10.0f;
		(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_IMP, OBJID::OBJ_ENEMY, vMuzzlePos, 1));

		_vec3 vMuzzlePos1 = vPos - vRight * 10.0f;
		(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_IMP, OBJID::OBJ_ENEMY, vMuzzlePos1, 1));

	}
	else //2페이즈
	{
		_vec3 vPos;
		m_pTransformCom->Get_Pos(&vPos);

		_vec3 vUp = { 0.f,1.f,0.f };
		_vec3 vRight;
		_vec3 vLook = m_vMoveLook;
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Cross(&vRight, &vUp, &vLook);
		_vec3 vMuzzlePos = vPos + vRight * 10.0f;
		(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_ARCHVILE, OBJID::OBJ_ENEMY, vMuzzlePos, 1));

		_vec3 vMuzzlePos1 = vPos - vRight * 10.0f;
		(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_ARCHVILE, OBJID::OBJ_ENEMY, vMuzzlePos1, 1));

	}
	
}

void CSpiderMastermind::Reset_Pattern3(const _float fTimeDelta)
{
	m_fSpeed = SPEED * 3;
	m_isPattern3On = TRUE;
	CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&m_vTargetPos);

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	_vec3 dir = m_vTargetPos - vPos;
	D3DXVec3Normalize(&dir, &dir);

	m_pColliderCom->Set_Active(false);

	m_vTargetDir = dir;
}

void CSpiderMastermind::Reset_Pattern4(const _float fTimeDelta)
{
}

void CSpiderMastermind::Pattern1(const _float fTimeDelta)
{
}

void CSpiderMastermind::Pattern2(const _float fTimeDelta)
{
}

void CSpiderMastermind::Pattern3(const _float fTimeDelta)
{
	//만약 애니타입이 NANRI가 아니고 애니메이션이 끝났다면
	if(m_eAnim_Type != NANRI && m_pAnimCom->Get_isFinAct())
		m_iTmp_Anim_Type = NANRI;

	//애니 타입을 NANRI로 바꾸기
	if(m_iTmp_Anim_Type == NANRI)
		m_eAnim_Type = NANRI;

	m_vMoveLook = m_vTargetDir;
}

void CSpiderMastermind::Pattern4(const _float fTimeDelta)
{
	_vec3 vPlayerPos;
	//CManagement::GetInstance()->Get_Player()->Get_Transform()->Get_Pos(&vPlayerPos);
	vPlayerPos  = static_cast<CDynamicCamera*>(CManagement::GetInstance()->Get_Player())->Get_Pos();
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	_vec3 dir = vPos - vPlayerPos;
	D3DXVec3Normalize(&dir, &dir);
	//CManagement::GetInstance()->Get_Player()->Get_Transform()->Move_Pos(&dir,5.f, fTimeDelta );
	vPlayerPos += dir * 5.f * fTimeDelta;

	static_cast<CDynamicCamera*>(CManagement::GetInstance()->Get_Player())->Set_Pos(vPlayerPos);
}

void CSpiderMastermind::Pattern_Timer(const _float fTimeDelta)
{
	m_fPattern_Timer += fTimeDelta;

	int steps = 0;

	while (m_fPattern_Timer >= m_fPattern_Delay)
	{
		m_fPattern_Timer -= m_fPattern_Delay;
		++steps;
	}

	if (steps > 0)
	{
		m_isAwake = true;
		m_iEnt_Pattern_Number ++;
		if(m_iEnt_Pattern_Number >= 8)
			m_iEnt_Pattern_Number = 0;	
	}
}

void CSpiderMastermind::Check_Anim_Fin(const _float& fTimeDelta)
{
	if (m_iFuncEntCnt == 0)									//이 함수에 처음 입장했다면
	{
		m_iFuncEntCnt++;
	}
	else
	{
		if (m_pAnimCom->Get_isFinAct())			//애니메이션이 끝났다면
		{
			m_isFinAnim = TRUE;
		}
		else
		{
			m_iFuncEntCnt++;
		}
	}
}
CSpiderMastermind* CSpiderMastermind::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSpiderMastermind* tmp = new CSpiderMastermind(pGraphicDev);

	return tmp;
}

CSpiderMastermind* CSpiderMastermind::Clone()
{
	CSpiderMastermind* PSpiderMastermind = new CSpiderMastermind(*this);

	if (FAILED(PSpiderMastermind->Ready_GameObject()))
	{
		Safe_Release(PSpiderMastermind);
		MSG_BOX("SpiderMastermind Create Failed");
		return nullptr;
	}

	return PSpiderMastermind;
}

void CSpiderMastermind::Free()
{
	Engine::CGameObject::Free();
}

