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
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);


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
#pragma endregion

#pragma region VARIABLE_SETTING
	m_fSightAngle = D3DXToRadian(80.f);
	m_fSightDistance = 50.f;

	m_fAtkSpeed = 4.f;
	m_iGameHp = 100;
	m_iAtkDmg = 3;

	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;

	std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<uint16_t> dist(0, 0xFFFF);
	m_iPattern = dist(rng);

	Calc_Pattern();

	for (int i = 0; i < 8; i++)
	{
		cout << m_PattenArr[i] << endl;
	}
	_vec3 vSize{ 10.f, 10.f, 10.f };
	m_pTransformCom->Set_Scale(vSize);
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

#pragma endregion

	return S_OK;
}

_int CSpiderMastermind::Update_GameObject(const _float& fTimeDelta)
{
//	cout << "스파이더 마스터마인드 픎레이어 발견 : " << m_isDetect_Player << endl;
	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();
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

void CSpiderMastermind::Set_OnTerrain()
{
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		_vec3 pos;
		m_pTransformCom->Get_Pos(&pos);

		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		_vec3 onPosition{ 0.f, 0.f, 0.f };
		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{

			m_pTransformCom->Set_Pos(pos.x, onPosition.y + 10, pos.z);
			break;
		}
	}
}

void CSpiderMastermind::Attack(const _float fTimeDelta)
{
	CMonsterBase::Attack(fTimeDelta);
	Fire(fTimeDelta);
	switch (m_PattenArr[m_AtkCnt])
	{
	case 0:
		Pattern1(fTimeDelta);
		break;
	case 1:
		Pattern2(fTimeDelta);
		break;
	case 2:
		Pattern1(fTimeDelta);
		break;
	case 3:
		Pattern2(fTimeDelta);
		break;
	}
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

void CSpiderMastermind::Pattern1(const _float fTimeDelta)
{
	m_isShoot = TRUE;
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	CGameObject* tmp = nullptr;

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


}

void CSpiderMastermind::Pattern2(const _float fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	vPos.x += 5;
	(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_IMP, OBJID::OBJ_ENEMY, vPos, 1));

	vPos.x -= 10;
	(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_IMP, OBJID::OBJ_ENEMY, vPos, 1));
	//auto layerMap = CManagement::GetInstance()->Get_Scene()->Get_Layer();

	
}

void CSpiderMastermind::Pattern3(const _float fTimeDelta)
{
}

void CSpiderMastermind::Pattern4(const _float fTimeDelta)
{
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

