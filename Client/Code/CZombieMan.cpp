#include "pch.h"
#include "CZombieMan.h"
#include "CPrototypeManager.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"

CZombieMan::CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonsterBase(pGraphicDev)
{
}

CZombieMan::CZombieMan(const CMonsterBase& rhs)
	: CMonsterBase(rhs)
{
}

CZombieMan::~CZombieMan()
{
}

HRESULT CZombieMan::Ready_GameObject()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);


	if (FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_pColliderCom->Ready_CCollider(this, vPos, _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f),
		COLLAYERID::COL_ENEMY,
		COLLAYERID::COL_PLAYER | COLLAYERID::COL_WALL | COLLAYERID::COL_OBJECT);

#pragma region ANIM_SETTING

	//이동
	m_pAnimCom->Add_Animation(
		ANIM_MOVE,
		m_pTextureCom,
		m_pBufferCom,
		5,				//가로
		1,				//세로
		0,				//start frame
		4,				//end frame
		0.2f,
		true,
		false
	);

	//공격
	m_pAnimCom->Add_Animation(
		ANIM_ATK,
		m_pTextureCom,
		m_pBufferCom,
		4,
		1,
		0,
		3,
		0.2f,
		true,
		false
	);

	//일반 죽음
	m_pAnimCom->Add_Animation(
		ANIM_NDEAD,
		m_pTextureCom,
		m_pBufferCom,
		6,
		1,
		0,
		5,
		0.2f,
		true,
		false
	);

	//폭사
	m_pAnimCom->Add_Animation(
		ANIM_GDEAD,
		m_pTextureCom,
		m_pBufferCom,
		9,
		1,
		0,
		8,
		0.2f,
		true,
		false
	);

#pragma endregion

#pragma region VARIABLE_SETTING
	m_fSightAngle =D3DXToRadian( 80.f);
	m_fSightDistance		= 50.f;

	m_fAtkSpeed			= 4.f;
	m_iHp						= 20;
	m_iAtkDmg				= 3;
	m_fAtkDelay			= 0.f;

	m_fMoveDelay			= 0.f;
	m_fAtkDelay			= 0.f;

	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

#pragma endregion


	return S_OK;
}

_int CZombieMan::Update_GameObject(const _float& fTimeDelta)
{
	Thinker(fTimeDelta);

	CGameObject::Update_GameObject(fTimeDelta);

	Set_OnTerrain();
	Bill_Board();
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return 0;
}

void CZombieMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pColliderCom->Late_Update();
}

void CZombieMan::Render_GameObject(const _float& fTimeDelta)
{
	Check_Target();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	//	m_pTextureCom->Set_Texture(m_eCurAnimState);

	//죽었는지 가장 먼저 체크 -> 공격하는지 체크 -> 아니면 이동


	//if (m_isGibbingDead)
	//{
	//	m_pAnimCom->Play_Animation(ANIM_GDEAD, 0, fTimeDelta, m_eCurAnimState);
	//}
	//else if (m_isNormalDead)
	//{
	//	m_pAnimCom->Play_Animation(ANIM_NDEAD, 0, fTimeDelta, m_eCurAnimState);
	//}
	//else if (m_isAtk)
	//{
	//	m_pAnimCom->Play_Animation(ANIM_ATK, 0, fTimeDelta, m_eCurAnimState);
	//	m_isAtk = false;
	//}
	//else
	//{	
	//}

	m_pAnimCom->Play_Animation(ANIM_MOVE, 0, fTimeDelta, m_eCurAnimState);
	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CZombieMan::OnCollision(CCollider* _pOther)
{
	CCollisionManager::Collision_Rect_Resolve(_pOther, m_pColliderCom, CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));
}

void CZombieMan::OnCollisionEnter(CCollider* _pOther)
{
}

void CZombieMan::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CZombieMan::Add_Component()
{
	CMonsterBase::Add_Component();

	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_ZombieMan_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CZombieMan::Key_Input(const _float& fTimeDelta)
{

	if (GetAsyncKeyState(VK_LBUTTON) && m_pAnimCom->Get_isFinAct())
	{
		m_eCurAnimState = 8;
		m_isAtk = false;
		m_fAtkDelay = 0.f;
	}
	if (GetAsyncKeyState('1') && m_pAnimCom->Get_isFinAct())
	{
		m_eCurAnimState = 16;
		m_isGibbingDead = true;
	}
	else if (GetAsyncKeyState('2') && m_pAnimCom->Get_isFinAct())
	{
		m_eCurAnimState = 17;
		m_isNormalDead = true;
	}
}

CZombieMan* CZombieMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CZombieMan* tmp = new CZombieMan(pGraphicDev);

	if (FAILED(tmp->Ready_GameObject()))
	{
		Safe_Release(tmp);
		MSG_BOX("ZombieMan Create Failed");
		return nullptr;
	}

	return tmp;
}

void CZombieMan::Free()
{
	Engine::CGameObject::Free();
}


void CZombieMan::Attack_Player(const _float& fTimeDelta)
{
	m_fAtkDelay += fTimeDelta;

	if (m_fAtkDelay > m_fAtkSpeed)
	{
		m_eCurAnimState += 8;
		m_fAtkDelay = 0;
	}

}

void CZombieMan::Movement(const _float& fTimeDelta)
{
	if(!m_isAtk)
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook), 3.f, fTimeDelta);
}


void CZombieMan::Attack()
{
	
}

void CZombieMan::Thinker(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_fMoveDelay += fTimeDelta;
	if (m_fMoveDelay > 4.f)
	{
		Set_Move_Direction(&m_vMoveLook);
		m_fMoveDelay = 0.f;
	}
	//지금 플레이어가 보는 방향에 따른 몬스터 상태를 가져옴


	if (m_pAnimCom->Get_isFinAct())
	{
		m_eCurAnimState = (MONSTER_ANIM_STATE)Calc_AnimState(m_vMoveLook, vPos);
		m_isGibbingDead = false;
		m_isNormalDead = false;
	}

	std::cout << "vLook" << m_vMoveLook.x << ' ' << m_vMoveLook.y << ' ' << m_vMoveLook.z << ' ' << std::endl;
	//m_eCurAnimState = (MONSTER_ANIM_STATE)Calc_AnimState(m_vMoveLook, vPos);
	Movement(fTimeDelta);
	Attack_Player(fTimeDelta);


//인지 -> 정말 체크만 하고 Bool 값을 변경해주기
	//시야에 어떤게 들어오는지
		//시야에 플레이어가 들어오는지
			
	//내가 총에 맞았는지
		//맞았다
			//맞고 죽었는가?
				//죽었다
					//초과된 대미지는 몇인지?
						//30보다 크다 -> 폭사
						//30보다 작다 -> 일반사
				// 안죽었다
					//총알을 쏜 주체로 Look 고정 
		//안맞았다

	//시야에 플레이어가 안들어오고 총에 맞지 않았다
	//시야에 플레이어가 들어오고 총에 맞지 않았다.
	//시야에 플레이어가 들어오고 총에 맞았다.
	//시야에 플레이어가 안들어오고 총에 맞았다.

//행동

	



}

void CZombieMan::Manage_MoveState()
{

}

void CZombieMan::Change_Animation()
{
	switch (m_eCurAnimState)
	{
	case Engine::MONSTER_ANIM_MOVE_F:
		break;
	case Engine::MONSTER_ANIM_MOVE_FR:
		break;
	case Engine::MONSTER_ANIM_MOVE_R:
		break;
	case Engine::MONSTER_ANIM_MOVE_BR:
		break;
	case Engine::MONSTER_ANIM_MOVE_B:
		break;
	case Engine::MONSTER_ANIM_MOVE_BL:
		break;
	case Engine::MONSTER_ANIM_MOVE_L:
		break;
	case Engine::MONSTER_ANIM_MOVE_FL:
		break;
	case Engine::MONSTER_ANIM_ATK_F:
		break;
	case Engine::MONSTER_ANIM_ATK_FR:
		break;
	case Engine::MONSTER_ANIM_ATK_R:
		break;
	case Engine::MONSTER_ANIM_ATK_BR:
		break;
	case Engine::MONSTER_ANIM_ATK_B:
		break;
	case Engine::MONSTER_ANIM_ATK_BL:
		break;
	case Engine::MONSTER_ANIM_ATK_L:
		break;
	case Engine::MONSTER_ANIM_ATK_FL:
		break;
	case Engine::MONSTER_ANIM_GIBBINGDEATH:
		break;
	case Engine::MONSTER_ANIM_NORMALDEATH:
		break;
	case Engine::MONSTER_ANIM_END:
		break;
	default:
		break;
	}
}


void CZombieMan::Bill_Board()
{
	_matrix	matWorld, matView, matBill;

	//그 오브젝트에 월드 행렬 구해오기
	m_pTransformCom->Get_World(&matWorld);

	//뷰행렬을 가져오기
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	// 뷰 스페이스 -> 카메라 월드 행렬의 역행렬
	//뷰행렬에 y 회전만 가져오기
	//뷰행렬의 역행렬은 카메라의 월드 행렬
	//지금 y축 회전은 월드 행렬이고
	//나머지는 뷰스페이스 행렬
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);		// 카메라가 갖고 있는 y회전의 역행렬 -역행렬> 카메라의 월드 상태의 y축 회전 행렬

	matWorld = matBill * matWorld;

	m_pTransformCom->Set_World(&matWorld);

}

