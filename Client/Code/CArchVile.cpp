#include "pch.h"
#include "CArchVile.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CBullet.h"
#include "CTerrainSector.h"
#include <iostream>

CArchVile::CArchVile(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CArchVile::CArchVile(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CArchVile::~CArchVile()
{
}

HRESULT CArchVile::Ready_GameObject()
{

	m_iMonsterType = OBJPROTOTYPE::ENEMY_ARCHVILE;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vPos;
	_vec3 vScale = { 3.f, 3.f, 3.f };

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
		13,
		1,
		0,
		12,
		0.15f,
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

	m_fAtkSpeed = rand() % 4 + 2;
	m_iGameHp = 700;
	m_iAtkDmg = 30;

	m_fSpeed = 12.f;
	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

	m_pTransformCom->Set_Pos(52.f, 10.f, 10.f);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
#pragma endregion

	CMonsterBase::Ready_GameObject();
	return S_OK;
}

_int CArchVile::Update_GameObject(const _float& fTimeDelta)
{

	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();

	return 0;
}

void CArchVile::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::LateUpdate_GameObject(fTimeDelta);
}

void CArchVile::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
}



void CArchVile::Attack(const _float fTimeDelta)
{
	m_isShoot = TRUE;
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	CGameObject* tmp = nullptr;

	if (FAILED(CManagement::GetInstance()->Set_GameLogicObject(OBJPROTOTYPE::ENEMY_ARCHVILE_BULLET, OBJID::OBJ_BULLET, vPos, this, tmp)))
	{
		int a = 0;
	}
	if (tmp == nullptr)
		return;

	tmp->Set_Dead(FALSE);
	tmp->Get_Transform()->Set_Pos(vPos.x, vPos.y, vPos.z);
	dynamic_cast<CBullet*>(tmp)->Set_InitDir(m_vMoveLook);

}

HRESULT CArchVile::Add_Component()
{
	CMonsterBase::Add_Component();

	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ArchVile_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CArchVile* CArchVile::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CArchVile* tmp = new CArchVile(pGraphicDev);

	return tmp;
}

CArchVile* CArchVile::Clone()
{
	CArchVile* PArchVile = new CArchVile(*this);

	if (FAILED(PArchVile->Ready_GameObject()))
	{
		Safe_Release(PArchVile);
		MSG_BOX("ArchVile Create Failed");
		return nullptr;
	}

	return PArchVile;
}

void CArchVile::Free()
{
	Engine::CGameObject::Free();
}

