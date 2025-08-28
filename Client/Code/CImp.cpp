#include "pch.h"
#include "CImp.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CBullet.h"


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
	//m_iMonsterType = OBJPROTOTYPE::ENEMY_IMP;
	CMonsterBase::Ready_GameObject();

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(52.f, 0.f, 10.f);

	_vec3 vPos;
	_vec3 vScale = { 2.f, 2.f, 2.f };

	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(2.f, 3.f, 2.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_ENEMY,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT | COL_BULLET);

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

	m_fAtkSpeed = 4.f;
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

	return S_OK;
}

_int CImp::Update_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Update_GameObject(fTimeDelta);
	Bill_Board();
	Set_OnTerrain();
	return 0;
}

void CImp::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::LateUpdate_GameObject(fTimeDelta);
}

void CImp::Render_GameObject(const _float& fTimeDelta)
{
	CMonsterBase::Render_GameObject(fTimeDelta);
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

