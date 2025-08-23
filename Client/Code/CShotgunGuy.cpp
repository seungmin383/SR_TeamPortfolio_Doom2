#include "pch.h"
#include "CShotgunGuy.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

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

	m_pTransformCom->Set_Pos(65.f, 0.f, 10.f);
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.5f, 0.f),
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

	m_fAtkSpeed = 4.f;
	m_iHp = 20;
	m_iAtkDmg = 30;

	m_fAtkDelay = 0.f;
	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	m_iGibDeadPlus = 24;
	m_iNorDeadPlus = 25;
	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
#pragma endregion

	return S_OK;
}

_int CShotgunGuy::Update_GameObject(const _float& fTimeDelta)
{

	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();
	return 0;
}

void CShotgunGuy::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::LateUpdate_GameObject(fTimeDelta);
}

void CShotgunGuy::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
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

