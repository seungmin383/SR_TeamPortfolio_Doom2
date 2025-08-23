#include "pch.h"
#include "CSpiderMastermind.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

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
	//AllocConsole();

	//FILE* fp;
	//freopen_s(&fp, "CONOUT$", "w", stdout);
	//freopen_s(&fp, "CONOUT$", "w", stderr);


	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(60.f, 0.f, 10.f);
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);
	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(10.f, 10.f, 10.f), _vec3(0.f, -5.f, 0.f),
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
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	Engine::CTerrainTexSector* pTerrainBufferCom =
		static_cast<CTerrainTexSector*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Set_Pos(pos.x, height + 10.f, pos.z);
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

