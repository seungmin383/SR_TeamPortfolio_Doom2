#include "pch.h"
#include "CMonsterBase.h"
#include "CManagement.h"

#include <iostream>


CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_fSightAngle(0.f), m_fSightDistance(0.f),
	m_isGibbingDead(false),
	m_isNormalDead(false),
	m_isAtk(false),
	m_eCurAnimState(0)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	:CCharacter(rhs), m_fSightAngle(m_fSightAngle), m_fSightDistance(m_fSightDistance),
	m_isGibbingDead(rhs.m_isGibbingDead),
	m_isNormalDead(rhs.m_isNormalDead),
	m_isAtk(rhs.m_isAtk),
	m_eCurAnimState(rhs.m_eCurAnimState)
{
}

CMonsterBase::~CMonsterBase()
{
}

void CMonsterBase::Debug_Triangle(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2)
{
	VTXCOL pVertex[3] = {
	{ _Pos, D3DCOLOR_ARGB(255, 255, 255, 255) },
	{ _Point2, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _Point1, D3DCOLOR_ARGB(255, 255, 255, 255)}
	};

	/*VTXCOL pVertex[3] = {
		{ _vec3{ -1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  0.0f,  1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) }
	};*/

	m_pGraphicDev->SetFVF(FVF_COL); // 정점 포맷 설정
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVertex, sizeof(VTXCOL));
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMonsterBase::Debug_Rectangle(_vec3 _LeftTop, _vec3 _RightTop, _vec3 _RightBottom, _vec3 _LeftBottom)
{
	VTXCOL pVertex[6] = {
	{ _LeftTop, D3DCOLOR_ARGB(255, 255, 255, 255) },
	{ _RightTop, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _RightBottom, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _LeftTop, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _RightBottom, D3DCOLOR_ARGB(255, 255, 255, 255)},
	{ _LeftBottom, D3DCOLOR_ARGB(255, 255, 255, 255)},
	};

	/*VTXCOL pVertex[3] = {
		{ _vec3{ -1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  1.0f, -1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  0.0f,  1.0f, -10.0f }, D3DCOLOR_XRGB(255, 255, 255) }
	};*/

	m_pGraphicDev->SetFVF(FVF_COL); // 정점 포맷 설정
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, pVertex, sizeof(VTXCOL));
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_vec3 CMonsterBase::Set_Move_Direction(_vec3* _Look)
{
	if(_Look == nullptr)
		m_vtmpLook = *_Look;

	//랜덤으로 0~7의 숫자를 뽑기
	_int RandMove = rand() % 8;
	_matrix RotMat;
	D3DXMatrixIdentity(&RotMat);

	//각 숫자에 맞는 방향으로 이동하기
	switch (RandMove)
	{
	//앞으로
	case 0:
		break;
	//왼쪽 앞으로
	case 1:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(45.f));		break;
	//왼쪽으로
	case 2:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(90.f));		break;
	//왼쪽 뒤로
	case 3:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(135.f));	break;
	//뒤로
	case 4:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(180.f));	break;
	//오른쪽 뒤로
	case 5:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-135.f));	break;
	//오른쪽으로
	case 6:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-90.f));	break;
	//오른쪽 앞으로
	case 7:		D3DXMatrixRotationY(&RotMat, D3DXToRadian(-45.f));	break;
	default:
		break;
	}

	D3DXVec3Normalize(&m_vtmpLook, &m_vtmpLook);
	D3DXVec3TransformCoord(&m_vtmpLook, &m_vtmpLook, &RotMat);
	if(_Look != nullptr)
		*_Look = m_vtmpLook;

	return m_vtmpLook;
}

//플레이어와 몬스터의 각도를 구해주는 함수
_float CMonsterBase::Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos)
{
	// 몬스터 위치에서 플레이어 위치를 뺴서 플레이어가 몬스터를 바라보는 벡터를 만들기
	_vec3 PlayerPos;
	_matrix CameraMat;
	D3DXMatrixIdentity(&CameraMat);

	//뷰행렬 가져오고 역행렬로 만들어서 카메라의 월드 행렬을 만듬
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &CameraMat);
	D3DXMatrixInverse(&CameraMat, NULL, &CameraMat);

	//카메라의 위치를 가져오고
	PlayerPos.x = CameraMat._41;
	PlayerPos.y = CameraMat._42;
	PlayerPos.z = CameraMat._43;

	//플레이어가 몬스터를 바라보는 방향의 벡터를 구함
	_vec3 PlayerLook = PlayerPos - vPos;

	//정규화 해주고
	D3DXVec3Normalize(&PlayerLook, &PlayerLook);
	D3DXVec3Normalize(&vLook, &vLook);
	
	float angleLook = atan2f(vLook.z, vLook.x);
	float angleToPlayer = atan2f(PlayerLook.z, PlayerLook.x);

	float fAngle = D3DXToDegree(angleToPlayer - angleLook);

	if (fAngle > 180.f)
		fAngle -= 360.f;
	if (fAngle < -180.f)
		fAngle += 360.f;
	
	return fAngle;
}
//플레이어와 몬스터의 각도를 이용해서 몬스터가 보여지는 애니메이션 상태를 정해줌
_int CMonsterBase::Calc_AnimState(_vec3 vLook, _vec3 vPos)
{
	_float fAngle = Calc_AngleToPlayer(vLook, vPos);

	if (fAngle >= 0.f && fAngle < 30.f)
		return MONSTER_ANIM_MOVE_F;
	else if (fAngle >= 30.f && fAngle < 60.f)
		return MONSTER_ANIM_MOVE_FR;
	else if (fAngle >= 60.f && fAngle < 90.f)
		return MONSTER_ANIM_MOVE_R;
	else if (fAngle >= 90.f && fAngle < 120.f)
		return MONSTER_ANIM_MOVE_R;
	else if (fAngle >= 120.f && fAngle < 150.f)
		return MONSTER_ANIM_MOVE_BR;
	else if (fAngle >= 150.f && fAngle <= 180.f)
		return MONSTER_ANIM_MOVE_B;

	else if (fAngle < 0.f && fAngle >= -30.f)
		return MONSTER_ANIM_MOVE_F;
	else if (fAngle < -30.f && fAngle >= -60.f)
		return MONSTER_ANIM_MOVE_FL;
	else if (fAngle < -60.f && fAngle >= -90.f)
		return MONSTER_ANIM_MOVE_L;
	else if (fAngle < -90.f && fAngle >= -120.f)
		return MONSTER_ANIM_MOVE_L;
	else if (fAngle < -120.f && fAngle >= -150.f)
		return MONSTER_ANIM_MOVE_BL;
	else if (fAngle < -150.f && fAngle >= -180.f)
		return MONSTER_ANIM_MOVE_B;
}

void CMonsterBase::Thinker1(const _float& fTimeDelta)
{
	Perception();										//인지

	Decision();

	Action(fTimeDelta);
}

void CMonsterBase::Perception()
{
	if (m_isDetect_Player = Check_Target())							//시야에 플레이어가 들어오는가?
	{
	}
														

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
}

#pragma region FUNCTION_FOR_PERCEPTION
//1. 시야에 플레이어가 들어왔는지 확인하는 함수
_bool CMonsterBase::Check_Target()
{
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

		D3DXMatrixRotationY(&RotMat, D3DXToRadian(45.f));							//왼쪽으로 45도 각도 -> RF
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
			return true;
		else
			return false;
}

#pragma endregion


void CMonsterBase::Decision()
{
}

void CMonsterBase::Action(const _float& fTimeDelta)
{

}

void CMonsterBase::Set_OnTerrain()
{
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);

	Engine::CTerrainTex* pTerrainBufferCom =
		static_cast<CTerrainTex*>
		(CManagement::GetInstance()->Get_Component(ID_STATIC, LAYERID::LAYER_ENVIRONMENT, OBJID::OBJ_TERRAIN, L"Com_Buffer"));

	_float height = m_pCalculatorCom->Compute_HeightOnTerrain(&pos, pTerrainBufferCom->Get_VtxPos(), VTXCNTX, VTXCNTZ, VTXITV);

	m_pTransformCom->Set_Pos(pos.x, height + 2.f, pos.z);
}

HRESULT CMonsterBase::Add_Component()
{
	CComponent* pComponent = NULL;

	//Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_RectSpriteTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//MoveTransform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_MoveTransform", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });


	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CMonsterBase::Free()
{
	Engine::CCharacter::Free();
}