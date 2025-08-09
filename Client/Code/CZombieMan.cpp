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
	m_fSightDistance = 50.f;

	m_fAtkSpeed = 4.f;
	m_iHp = 20;
	m_iAtkDmg = 3;
	m_fAtkDelay = 0.f;

	m_fMoveDelay = 0.f;
	m_fAtkDelay = 0.f;

	//초기 룩으로 세팅해주고
	m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);

#pragma endregion


	return S_OK;
}

_int CZombieMan::Update_GameObject(const _float& fTimeDelta)
{
	//Thinker(fTimeDelta);

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
	if (m_isGibbingDead)
	{
		m_pAnimCom->Play_Animation(ANIM_GDEAD, 0, fTimeDelta, m_eCurAnimState);
	}
	else if (m_isNormalDead)
	{
		m_pAnimCom->Play_Animation(ANIM_NDEAD, 0, fTimeDelta, m_eCurAnimState);
	}
	else if (m_isAtk)
	{
		m_pAnimCom->Play_Animation(ANIM_ATK, 0, fTimeDelta, m_eCurAnimState);
		//m_isAtk = false;
	}
	else
	{	
		m_pAnimCom->Play_Animation(ANIM_MOVE, 0, fTimeDelta, m_eCurAnimState);
	}
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
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectSpriteTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//MoveTransform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_MoveTransform", pComponent });

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_ZombieMan_Texture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });


	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });


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

void CZombieMan::Set_OnTerrain()
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	Engine::CTerrainTex* pTerrainBufferCom =
		static_cast<CTerrainTex*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Set_Pos(pos.x, height + 2.f, pos.z);
}

CZombieMan* CZombieMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CZombieMan* tmp = new CZombieMan(pGraphicDev);

	return tmp;
}

CZombieMan* CZombieMan::Clone()
{
	CZombieMan* pZombieMan = new CZombieMan(*this);

	if (FAILED(pZombieMan->Ready_GameObject()))
	{
		Safe_Release(pZombieMan);
		MSG_BOX("ZombieMan Create Failed");
		return nullptr;
	}

	return pZombieMan;
}

void CZombieMan::Free()
{
	Engine::CGameObject::Free();
}

//몬스터의 시야 안에 플레이어가 있는지 확인
void CZombieMan::Check_Target()
{
#pragma region TRIANGLE
	//_vec3 vPos;
//m_pTransformCom->Get_Pos(&vPos);
////몬스터 Look 방향
////_vec3 vLook;
////m_pTransformCom->Get_Info(INFO_LOOK, &m_vMoveLook);


//m_vMoveLook.y = 0.f;
////Normalize
//D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);

////Look 방향에 시야 길이만큼 곱해주기
//m_vMoveLook *= m_fSightDistance;

////좌우 경계 벡터
//_vec3 vLeftBorderLine = m_vMoveLook;
//_vec3 vRightBorderLine = m_vMoveLook;

////왼쪽 경게 벡터
//_matrix yRotMat;
//D3DXMatrixRotationY(&yRotMat, -(m_fSightAngle / 2.f));
//D3DXVec3TransformNormal(&vLeftBorderLine, &vLeftBorderLine, &yRotMat);

////초기화 해주고
//D3DXMatrixIdentity(&yRotMat);

////오른쪽 경계 벡터
//D3DXMatrixRotationY(&yRotMat, (m_fSightAngle / 2.f));
//D3DXVec3TransformNormal(&vRightBorderLine, &vRightBorderLine, &yRotMat);

//_vec3 vLeftPos = vPos + vLeftBorderLine;
//_vec3 vRightPos = vPos + vRightBorderLine;


//Debug_Line(vPos, vLeftPos, vRightPos);


#pragma endregion

#pragma region RECTANGLE
	_vec3 vPos;																									//내 위치
	m_pTransformCom->Get_Pos(&vPos);

	_vec3 vUp{ 0.f, 1.f, 0.f };																				//업 벡터(룩이랑 외적용)

	_vec3 vLB;																									//왼쪽 아래 경계점
	_vec3 vRB;																									//오른쪽 아래 경계점

	m_vMoveLook.y = 0;																					//y고정

	_vec3 vLF = m_vMoveLook;																			//왼쪽 위 경계점
	_vec3 vRF = m_vMoveLook;																		//오른쪽 위 경계점

	_matrix RotMat;																							//벡터 회전용 매트릭스
	D3DXMatrixIdentity(&RotMat);																	//초기화

	//LF, RF
	D3DXMatrixRotationY(&RotMat, D3DXToRadian(-45.f));							//왼쪽으로 45도 각도 -> LF
	D3DXVec3Normalize(&vLF, &vLF);																//정규화
	D3DXVec3TransformCoord(&vLF, &vLF, &RotMat);									//방향성 정해주기

	D3DXMatrixIdentity(&RotMat);																	//사용한 매트릭스 초기화

	D3DXMatrixRotationY(&RotMat, D3DXToRadian( 45.f));							//왼쪽으로 45도 각도 -> RF
	D3DXVec3Normalize(&vRF, &vRF);																//정규화
	D3DXVec3TransformCoord(&vRF, &vRF, &RotMat);									//방향성 정해주기

	vLF *= m_fSightDistance;																				//범위 지정해주기
	vRF *= m_fSightDistance;																			//

	//LB, RB
	D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook);							//정규화
	D3DXVec3Normalize(&vUp, &vUp);															//정규화
	
	D3DXVec3Cross(&vRB, &vUp, &m_vMoveLook);										//Up LooK 외적 -> 오른쪽 벡터
	D3DXVec3Cross(&vLB, &m_vMoveLook, &vUp);										//Look Up 외적 -> 왼쪽 벡터

	D3DXVec3Normalize(&vRB, &vRB);															//정규화
	D3DXVec3Normalize(&vLB, &vLB);																//정규화

	vRB *= 20.f;																									//범위 지정해주기
	vLB *= 20.f;																									//

	Debug_Rectangle(																						//시야 범위 시각화
		vPos + vLF,
		vPos + vRF,
		vPos + vRB,
		vPos + vLB
	);

	// 플레이어가 범위에 있는가
	_vec3 vPlayerPos;																							//플레이어 위치
	_matrix ViewMat;																							//카메라 월드 행렬 구하기 위한

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMat);						//뷰 스페이스 행렬 가져오기

	D3DXMatrixInverse(&ViewMat, NULL, &ViewMat);									//카메라의 월드 행렬

	vPlayerPos.x = ViewMat._41;																		//카메라 = 플레이어
	vPlayerPos.y = ViewMat._42;
	vPlayerPos.z = ViewMat._43;

	_vec3 vToPlayer = vPlayerPos - vPos;
	vToPlayer.y = 0.f;
	D3DXVec3Normalize(&vToPlayer, &vToPlayer);

	_vec3 vLook = m_vMoveLook;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	// cos(시야각 / 2) = cos(90도) = 0
	float fDot = D3DXVec3Dot(&vToPlayer, &vLook);

	if (fDot >= 0.f && D3DXVec3Length((&vToPlayer)) <= m_fSightDistance)
	{
		std::cout << "플레이어 발견" << std::endl;
		m_vMoveLook;
		m_isAtk = true;
	}
	else
		std::cout << "플레이어 발견 못함" << std::endl;
#pragma endregion
}

void CZombieMan::Attack_Player(const _float& fTimeDelta)
{
	m_fAtkDelay += fTimeDelta;

	if (m_fAtkDelay > m_fAtkSpeed)
	{
		m_eCurAnimState += 8;
		m_isAtk = true;
	}

}

void CZombieMan::Movement(const _float& fTimeDelta)
{
	if(!m_isAtk)
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&m_vMoveLook, &m_vMoveLook), 3.f, fTimeDelta);
}


void CZombieMan::Thinker(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_fMoveDelay += fTimeDelta;
	if (m_fMoveDelay > 4.f)
	{
		Set_Direction(&m_vMoveLook);
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
	m_eCurAnimState = (MONSTER_ANIM_STATE)Calc_AnimState(m_vMoveLook, vPos);
	Movement(fTimeDelta);
	Attack_Player(fTimeDelta);

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


void CZombieMan::Attack()
{
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

