#include "pch.h"
#include "CMonsterBase.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "Engine_Enum.h"
#include "CWeaponManager.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include <iostream>
#include "CStatsManager.h"
#include "CTerrainSector.h"
#include "EffectManager.h"
#include "CSoundManager.h"
#include "CBfgBullet.h"

CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_fSightAngle(0.f), m_fSightDistance(0.f),
	m_isGibbingDead(false),
	m_isNormalDead(false),
	m_isAtk(false),
	m_eCurAnimState(0),
	m_fAtkDelay(0.f), m_fMoveDelay(0.f), m_fChaseDelay(0.f),
	m_isRecognize_Player(false)
{
}

//CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
//	:CCharacter(rhs), m_fSightAngle(rhs.m_fSightAngle), m_fSightDistance(rhs.m_fSightDistance),
//	m_isGibbingDead(rhs.m_isGibbingDead),
//	m_isNormalDead(rhs.m_isNormalDead),
//	m_isAtk(rhs.m_isAtk),
//	m_eCurAnimState(rhs.m_eCurAnimState),
//	m_fAtkDelay(0.f), m_fMoveDelay(0.f), m_fChaseDelay(0.f),
//	m_isRecognize_Player(false)
//{
//}
CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	: CCharacter(rhs)
	// --- 컴포넌트 포인터: 복사 금지(별도 Add_Component에서 세팅) ---
	, m_pCalculatorCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pAnimCom(nullptr)

	// --- AI 상태 ---
	, m_iCurThought(0)
	, m_iPreThought(0)

	// --- 애니 상태/타입 ---
	, m_eCurAnimState(rhs.m_eCurAnimState)
	, m_ePreAnimState(rhs.m_eCurAnimState)
	, m_eAnim_Type(ANIM_MOVE)

	// --- 이동 벡터 ---
	, m_vMoveLook(rhs.m_vMoveLook)
	, m_vtmpLook(rhs.m_vtmpLook)

	// --- 시야 파라미터(설정값은 복사) ---
	, m_fSightAngle(rhs.m_fSightAngle)
	, m_fSightDistance(rhs.m_fSightDistance)

	// --- 타이머/카운터(런타임 값은 리셋)---
	, m_fAtkDelay(0.f)
	, m_fMoveDelay(0.f)
	, m_fChaseDelay(0.f)
	, m_fHitDelay(0.f)
	, m_AtkEnterCnt(0)
	, m_HitEnterCnt(0)

	// --- 동작 플래그(런타임 값은 리셋) ---
	, m_isMove(false)
	, m_isAtk(false)
	, m_isWallHit(false)

	// --- 인지/인식 플래그 ---
	, m_isDetect_Player(false)
	, m_isDetected_Player(false)
	, m_isRecognize_Player(false)

	// --- 피격/사망/유휴 ---
	, m_isHit(false)
	, m_isDead(FALSE)
	, m_isNormalDead(false)
	, m_isGibbingDead(false)
	, m_isIdle(TRUE)

	// --- 통계/드랍 1회성 가드 ---
	, m_bCountedDeath(false)
	, m_bDropDone(false)

	// --- 플레이어 위치 캐시 ---
	, m_PlayerPos{ 0.f, 0.f, 0.f }

	// --- 애니 시트 오프셋(설정값은 복사) ---
	, m_iGibDeadPlus(rhs.m_iGibDeadPlus)
	, m_iNorDeadPlus(rhs.m_iNorDeadPlus)

	// --- 특성 플래그/보스 여부(설정값은 복사) ---
	, m_isPinky(rhs.m_isPinky)
	, m_isBoss(rhs.m_isBoss)
	, m_isShoot(false)                 // ★ 공격 1회 트리거 가드 리셋

	// --- 타입/각도(설정값은 복사) ---
	, m_iMonsterType(rhs.m_iMonsterType)
	, m_fTargetAngle(rhs.m_fTargetAngle)
	, m_fCurAngle(rhs.m_fCurAngle)

	// --- 타겟/체력/기타 ---
	, m_pAttackTarget(nullptr)         // 기본은 플레이어 타겟
	, m_iPreHp(rhs.m_iGameHp)          // 직후 Calc_Hp 로직 일관성
	, m_iHeight(rhs.m_iHeight)

	// --- 보스 패턴용 ---
	, m_AtkCnt(0)
	, m_isFirstEnt(FALSE)
	, m_isPattern3On(FALSE)
{
}

CMonsterBase::~CMonsterBase()
{
}


HRESULT         CMonsterBase::Ready_GameObject()
{
	m_iPreHp = m_iGameHp;

	return S_OK;
}

_int CMonsterBase::Update_GameObject(const _float& fTimeDelta)
{
	Thinker(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	CGameObject::Update_GameObject(fTimeDelta);
	if (m_isGibbingDead || m_isNormalDead)
	{
		return -1;
	}

	return 0;
}

void                 CMonsterBase::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimCom->Play_Animation(m_eAnim_Type, 0, fTimeDelta, m_eCurAnimState);
	m_pColliderCom->Late_Update();

}

void                 CMonsterBase::Render_GameObject(const _float& fTimeDelta)
{

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);			//알파 테스트를 하겠다.

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture(m_eCurAnimState);
	m_pBufferCom->Render_Buffer();
	//충돌체
	m_pColliderCom->Render(g_colRend);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CMonsterBase::OnCollision(CCollider* _pOther)
{
	CCollisionManager::Collision_Rect_Resolve(_pOther, m_pColliderCom, CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));
}

void CMonsterBase::OnCollisionEnter(CCollider* _pOther)
{
	CCollisionManager::Collision_Rect_Resolve(_pOther, m_pColliderCom, CTimerManager::GetInstance()->Get_TimeDelta(L"Timer_60"));
	if (_pOther->Get_ColLayer() == COLLAYERID::COL_WALL)
	{
		m_vMoveLook = -m_vMoveLook;
	}

	if (_pOther->Get_ColLayer() == COLLAYERID::COL_BULLET)
	{
		CGameObject* pOther;
		pOther = _pOther->Get_Owner();
		if (dynamic_cast<CBfgBullet*>(pOther) != nullptr)
		{
			_vec3 vPos;
			m_pTransformCom->Get_Pos(&vPos);
			EffectManager::GetInstance()->Start_Effect(vPos, EFFECTID::BFG_EFFECT);
		}
	}
}

void CMonsterBase::OnCollisionExit(CCollider* _pOther)
{
}
_vec3 CMonsterBase::Set_Move_Direction(_vec3* _Look, _float _DelayTime, _float _fTimedelta)
{
	if (_Look != nullptr)
		m_vtmpLook = *_Look;

	_matrix RotMat;
	D3DXMatrixIdentity(&RotMat);

	// --- 1) 랩 고려한 각도 차이(delta) 계산 ---
	float delta = m_fTargetAngle - m_fCurAngle;
	// -360~360로 먼저 접고
	delta = fmodf(delta, 360.f);
	// -180~180으로 재래핑
	if (delta > 180.f) delta -= 360.f;
	if (delta < -180.f) delta += 360.f;

	// --- 2) 목표각을 향해 한 스텝 이동 (시간 기반 회전 속도 권장) ---
	// 현재는 고정 2도/프레임. 시간기반 원하면 float turnSpeed=180.f; float step=turnSpeed*_fTimedelta;
	float step = 2.f;
	if (delta > step)      m_fCurAngle += step;
	else if (delta < -step) m_fCurAngle -= step;
	else                    m_fCurAngle = m_fTargetAngle; // 스냅

	// --- 3) 현재각 래핑 (-180~180) ---
	m_fCurAngle = fmodf(m_fCurAngle, 360.f);
	if (m_fCurAngle > 180.f) m_fCurAngle -= 360.f;
	if (m_fCurAngle < -180.f) m_fCurAngle += 360.f;

	// --- 4) 도달 판정 → 새 목표각 선택 ---
	// (delta를 다시 계산하지 않고도 사용 가능)
	if (fabsf(delta) <= 2.f || m_isWallHit)
	{
		int RandMove = rand() % 14;
		switch (RandMove)
		{
		case 0:   m_fTargetAngle = 0.f;			break;
		case 1:   m_fTargetAngle = 30.f;			break;
		case 2:   m_fTargetAngle = 60.f;			break;
		case 3:   m_fTargetAngle = 90.f;			break;
		case 4:   m_fTargetAngle = 120.f;		break;
		case 5:   m_fTargetAngle = 150.f;		break;
		case 6:   m_fTargetAngle = 180.f;		break;
		case 7:   m_fTargetAngle = -30.f;		break;
		case 8:   m_fTargetAngle = -60.f;		break;
		case 9:   m_fTargetAngle = -90.f;		break;
		case 10: m_fTargetAngle = -120.f;		break;
		case 11: m_fTargetAngle = -150.f;		break;
		case 12: m_fTargetAngle = -180.f;		break;
		case 13: m_fTargetAngle = 120.f;		break;
		}
		// 목표각도도 래핑
		m_fTargetAngle = fmodf(m_fTargetAngle, 360.f);
		if (m_fTargetAngle > 180.f) m_fTargetAngle -= 360.f;
		if (m_fTargetAngle < -180.f) m_fTargetAngle += 360.f;
	}
	// --- 5) 회전 적용 후 이동 방향 벡터 갱신 ---
	const _vec3 vForward(0.f, 0.f, 1.f);
	D3DXMatrixRotationY(&RotMat, D3DXToRadian(m_fCurAngle));
	D3DXVec3TransformNormal(&m_vtmpLook, &vForward, &RotMat);
	m_vtmpLook.y = 0.f;
	D3DXVec3Normalize(&m_vtmpLook, &m_vtmpLook);

	if (_Look != nullptr)
		*_Look = m_vtmpLook;

	return m_vtmpLook;
}

void CMonsterBase::Thinker(const _float& fTimeDelta)
{
	m_iCurThought = Perception(fTimeDelta);										//인지

	Action(fTimeDelta, m_iCurThought);												//행동

	m_iPreThought = m_iCurThought;
}

_int CMonsterBase::Perception(const _float& fTimeDelta)
{
	m_isDead = Check_Dead();
	Calc_Hp();
	Check_Hit(fTimeDelta);
	Check_Shooting_Sound();
	m_isDetect_Player = Check_Target();
	m_isAtk = Check_Attack(fTimeDelta);

	if (!m_isRecognize_Player && m_isDetect_Player)
		m_isRecognize_Player = TRUE;

	if (m_isAtk && m_isDetect_Player)
		m_isDetected_Player = TRUE;

	if (m_isDetected_Player)
		m_isIdle = FALSE;

	//죽었는지
	if (m_isDead)
		return AI_DEAD;

	//피격당했는지
	else if (m_isHit)
		return AI_HIT;

	else if (m_isIdle)
	{
		return AI_MOVE;
	}

	//공격할 수 있는지
	else if (m_isAtk && m_isDetect_Player)
		return AI_ATTACK;

	//플레이어를 발견하고 따라가는지
	else if (m_isDetect_Player)
	{
		if (m_isBoss)
			return AI_MOVE;

		return AI_CHASE;
	}

	else//그냥 랜덤 이동할지
		return AI_MOVE;
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

	D3DXMatrixRotationY(&RotMat, D3DXToRadian(45.f));								//왼쪽으로 45도 각도 -> RF
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

	//Debug_Rectangle(																						//시야 범위 시각화
	//	vPos + vLF,
	//	vPos + vRF,
	//	vPos + vRB,
	//	vPos + vLB
	//);

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
	_vec3 vDistanceToPlayer = vToPlayer;

	D3DXVec3Normalize(&vToPlayer, &vToPlayer);

	_vec3 vLook = m_vMoveLook;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	// cos(시야각 / 2) = cos(90도) = 0
	float fDot = D3DXVec3Dot(&vToPlayer, &vLook);

	float distSq = D3DXVec3LengthSq(&vDistanceToPlayer);
	float sightSq = m_fSightDistance * m_fSightDistance;

	if ((fDot >= 0.f) && (distSq <= sightSq)) //플레이어가 범위 안에 들어왔다
	{
		return TRUE;
	}
	else //플레이어가 범위 밖에 있다
		return FALSE;

	return false;
}

_bool CMonsterBase::Check_Shooting_Sound()
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

	D3DXMatrixRotationY(&RotMat, D3DXToRadian(45.f));								//왼쪽으로 45도 각도 -> RF
	D3DXVec3Normalize(&vRF, &vRF);																//정규화
	D3DXVec3TransformCoord(&vRF, &vRF, &RotMat);									//방향성 정해주기

	vLF *= m_fSightDistance;																				//범위 지정해주기
	vRF *= m_fSightDistance;																			//

	//LB, RB
	D3DXMatrixRotationY(&RotMat, D3DXToRadian(-135.f));							//왼쪽으로 45도 각도 -> LF
	D3DXVec3Normalize(&vLF, &vLF);																//정규화
	D3DXVec3TransformCoord(&vLF, &vLF, &RotMat);									//방향성 정해주기

	D3DXMatrixIdentity(&RotMat);																	//사용한 매트릭스 초기화

	D3DXMatrixRotationY(&RotMat, D3DXToRadian(135.f));							//왼쪽으로 45도 각도 -> RF
	D3DXVec3Normalize(&vRF, &vRF);																//정규화
	D3DXVec3TransformCoord(&vRF, &vRF, &RotMat);									//방향성 정해주기

	vLF *= m_fSightDistance;																				//범위 지정해주기
	vRF *= m_fSightDistance;																			//
	//

	//Debug_Rectangle(																						//시야 범위 시각화
	//	vPos + vLF,
	//	vPos + vRF,
	//	vPos + vRB,
	//	vPos + vLB
	//);

	// 플레이어가 범위에 있는가
	_vec3 vPlayerPos;																							//플레이어 위치
	_matrix ViewMat;																							//카메라 월드 행렬 구하기 위한

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &ViewMat);						//뷰 스페이스 행렬 가져오기

	D3DXMatrixInverse(&ViewMat, NULL, &ViewMat);									//카메라의 월드 행렬

	vPlayerPos.x = ViewMat._41;																		//카메라 = 플레이어
	vPlayerPos.y = ViewMat._42;
	vPlayerPos.z = ViewMat._43;

	_vec3 vToPlayer = vPlayerPos - vPos;															//내가 플레이어를 바라보는
	vToPlayer.y = 0.f;
	_vec3 vDistanceToPlayer = vToPlayer;															// 스칼라 저장

	D3DXVec3Normalize(&vToPlayer, &vToPlayer);											//정규화

	_vec3 vLook = m_vMoveLook;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);


	float distSq = D3DXVec3LengthSq(&vDistanceToPlayer);
	float sightSq = m_fSightDistance * m_fSightDistance;

	if (distSq <= sightSq)
	{
		if (CWeaponManager::GetInstance()->Get_CurWeapon()->Get_WeaponState() == WEAPON_STATE::SHOT)
		{
			_matrix	player;
			D3DXMatrixIdentity(&player);
			m_pGraphicDev->GetTransform(D3DTS_VIEW, &player);
			_vec3 PlayerPos;
			PlayerPos.x = player._41;
			PlayerPos.y = player._42;
			PlayerPos.z = player._43;
			_vec3 vPos;
			m_pTransformCom->Get_Pos(&vPos);
			_vec3 look = vPlayerPos - vPos;
			D3DXVec3Normalize(&look, &look);
			if (!m_isBoss)
				m_vMoveLook = look;

			m_isDetected_Player = true;
			return true;
		}
		else
			return false;
	}
}

_bool CMonsterBase::Check_Dead()
{
	if (m_iGameHp <= 0)
	{
		m_bDead = true;
		if (m_iGameHp <= -30)
		{
			m_isGibbingDead = TRUE;
			m_isNormalDead = FALSE;

			// 킬 집계(08/24 김홍인)
			if (!m_bCountedDeath)
			{
				m_bCountedDeath = true;
				CStatsManager::I().OnEnemyKilled();
			}

			if (!m_bDropDone) {				// (08/25 수정 - 김홍인)
				Drop_Item();
				m_bDropDone = true;
			}

			return	TRUE;
		}
		m_isGibbingDead = FALSE;
		m_isNormalDead = TRUE;

		// 킬 집계
		if (!m_bCountedDeath)
		{
			m_bCountedDeath = true;

			CStatsManager::I().OnEnemyKilled();
		}

		//Drop_Item();
		if (!m_bDropDone) {				// (08/25 수정 - 김홍인)
			Drop_Item();
			m_bDropDone = true;
		}
		return TRUE;
	}
	return FALSE;
}

void CMonsterBase::Check_Hit(const _float& fTimeDelta)
{
	if (m_HitEnterCnt == 0)									//이 함수에 처음 입장했다면
	{
		m_HitEnterCnt++;
	}
	else
	{
		if (m_isHit && m_pAnimCom->Get_isFinAct())			//애니메이션이 끝났다면
		{
			m_HitEnterCnt = 0;
			m_fAtkDelay = 0.f;
			m_isHit = FALSE;
		}
		else
		{
			m_HitEnterCnt++;
		}
	}
}

void CMonsterBase::Calc_Hp()
{
	if (m_iGameHp != m_iPreHp)
	{
		if (m_isBoss)
		{
			_int a = rand() % 10;
			if (a > 7)
				m_isHit = TRUE;
			else
				m_isHit = FALSE;
		}
		else
			m_isHit = TRUE;

		m_iPreHp = m_iGameHp;
	}

	return;
}

_bool CMonsterBase::Check_Attack(const _float& fTimeDelta)
{
	m_fAtkDelay += fTimeDelta;

	if (m_fAtkDelay > m_fAtkSpeed)
	{
		if (m_AtkEnterCnt == 0)									//이 함수에 처음 입장했다면
		{
			m_AtkEnterCnt++;										//카운트만 올리고 return 
			return TRUE;
		}
		else
		{
			if (m_pAnimCom->Get_isFinAct())			//애니메이션이 끝났다면
			{
				m_AtkEnterCnt = 0;
				m_fAtkDelay = 0.f;
				m_isDetected_Player = FALSE;
				m_isShoot = FALSE;
				return FALSE;
			}
			else
			{
				m_AtkEnterCnt++;
				return TRUE;
			}
		}
	}
	else
		return FALSE;
}

#pragma endregion


void CMonsterBase::Action(const _float& fTimeDelta, _int _Thought)
{
	_vec3 vPos;
	m_pTransformCom->Get_Pos(&vPos);

	m_eCurAnimState = Calc_AnimState_byPlayer(m_vMoveLook, vPos);

	int tmpAnimType = m_eAnim_Type;

	switch (_Thought)
	{
		//죽었을 때 
	case AI_DEAD:
		m_isFirstEnt = FALSE;
		if (m_isGibbingDead)
		{
			m_eCurAnimState = m_iGibDeadPlus;
			m_eAnim_Type = GIB_DEATH;
		}
		else if (m_isNormalDead)
		{
			m_eCurAnimState = m_iNorDeadPlus;
			m_eAnim_Type = NOR_DEATH;
		}
		m_pColliderCom->Set_Active(false);
		break;

		//맞았을 때
	case AI_HIT:
		m_isFirstEnt = FALSE;
		if (!m_isPinky)
			m_eCurAnimState += 16;
		m_eAnim_Type = HIT;
		break;

		//공격할 때
	case AI_ATTACK:
		m_eCurAnimState += 8;
		m_eAnim_Type = ATK;

		if (!m_isShoot)
			Attack(fTimeDelta);

		break;

		//플레이어를 발견했을 때
	case AI_CHASE:
		m_isFirstEnt = FALSE;
		m_fChaseDelay += fTimeDelta;
		if (m_eCurAnimState != m_ePreAnimState)
			m_eAnim_Type = MOVE;

		if (m_fChaseDelay > 0.5f)
		{
			Monster_Look_Player(&m_vMoveLook);
			m_fChaseDelay = 0.f;
		}
		Movement(fTimeDelta);
		break;

		//아이들 상태
	case AI_MOVE:
		m_isFirstEnt = FALSE;
		if (m_eCurAnimState != m_ePreAnimState)
			m_eAnim_Type = MOVE;

		if (m_isRecognize_Player)
		{
			Set_Move_Direction(&m_vMoveLook, 1.f, fTimeDelta);
			Movement(fTimeDelta);
		}

		break;
	}


	if (tmpAnimType == NANRI)
		m_eAnim_Type = NANRI;

	m_ePreAnimState = m_eCurAnimState;
}

void CMonsterBase::Monster_Look_Player(_vec3* _vLook)
{
	_vec3 vPos;														//내위치
	m_pTransformCom->Get_Pos(&vPos);
	_vec3 vPlayerPos;												//플레이어 위치
	vPlayerPos = Get_PlayerPos();

	_vec3 tmp = vPlayerPos - vPos;
	D3DXVec3Normalize(&tmp, &tmp);
	*_vLook = tmp;
}

void CMonsterBase::Monster_Fire()
{
}

void CMonsterBase::Set_OnTerrain()
{
	_float fHighest(-999.f);
	_vec3 pos;
	m_pTransformCom->Get_Pos(&pos);
	_vec3 onPosition{ 0.f, 0.f, 0.f };
	_float vScaleY = m_pTransformCom->Get_Scale()->y;

	auto terrain = CManagement::GetInstance()->Get_Scene()->Get_Terrain();
	vector<CGameObject*> innerTerrain;

	for (auto tt : terrain)
	{
		if (CCollisionManager::InnerTerrainCheck(tt, pos))
			innerTerrain.push_back(tt);
	}

	for (auto ter : innerTerrain)
	{
		auto dterrain = dynamic_cast<CTerrainSector*>(ter);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{
			if (fHighest < onPosition.y)
				fHighest = onPosition.y;
		}
	}

	if (fHighest == -999.f)
		fHighest = 0.f;
	else
		int a = 1;

	m_pTransformCom->Set_Pos(pos.x, fHighest + vScaleY, pos.z);
}

void CMonsterBase::CalcInnerTerrain(CGameObject* terrain, _float& result, _vec3& onPos, _vec3& myPos, mutex& m)
{
	auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
	auto bufferCom = dterrain->Get_BufferCom();
	auto transformCom = dterrain->Get_Transform();

	m.lock();
	if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPos, &myPos))
	{
		if (result < onPos.y)
			result = onPos.y;
	}
	m.unlock();
}

void CMonsterBase::Movement(const _float fTimeDelta)
{
	//m_pTransformCom->Move_Pos(&m_vMoveLook, m_fSpeed, fTimeDelta);

	// Raycast part
	_vec3 vPos, vHitPos, vHitNormal, vLook;
	vLook = m_vMoveLook;
	vLook.y = 0.f;
	LAYERID eHitLayer = LAYERID::LAYER_END;

	m_pTransformCom->Get_Pos(&vPos);
	CCollisionManager::Raycast(vPos, vLook, vHitPos, vHitNormal, 2.f * m_pColliderCom->Get_Scale().x, &eHitLayer, this);

	if (eHitLayer != LAYERID::LAYER_ENVIRONMENT)
	{
		m_pTransformCom->Move_Pos(&vLook, m_fSpeed, fTimeDelta);
	}
	else
	{
		_vec3	vMoveForce = vLook * m_fSpeed * fTimeDelta;
		_float	fForce = D3DXVec3Dot(&vMoveForce, &vHitNormal);

		if (fForce < 0.f)
		{
			_vec3	vForce = fForce * vHitNormal;
			_vec3	vSlide = vMoveForce - vForce;
			D3DXVec3Normalize(&vSlide, &vSlide);

			m_pTransformCom->Move_Pos(&vSlide, m_fSpeed, fTimeDelta);
			m_pTransformCom->Move_Pos(&vHitNormal, 2.f * m_fSpeed, fTimeDelta);
		}
	}
}

void CMonsterBase::Attack(const _float fTimeDelta)
{

}

_int CMonsterBase::Calc_Death_Time(const _float fTimeDelta)
{
	m_fDeathTimer += fTimeDelta;

	if (m_fDeathTimer > 3.5f)
	{
		m_fDeathTimer = 0.f;
		return -1;
	}
	return 0;
}

_vec3 CMonsterBase::Get_PlayerPos()
{
	_matrix CameraMat;
	D3DXMatrixIdentity(&CameraMat);

	//뷰행렬 가져오고 역행렬로 만들어서 카메라의 월드 행렬을 만듬
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &CameraMat);
	D3DXMatrixInverse(&CameraMat, NULL, &CameraMat);

	//카메라의 위치를 가져오고
	m_PlayerPos.x = CameraMat._41;
	m_PlayerPos.y = CameraMat._42;
	m_PlayerPos.z = CameraMat._43;

	return m_PlayerPos;
}

//플레이어와 몬스터의 각도를 구해주는 함수
_float CMonsterBase::Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos)
{
	Get_PlayerPos();

	//플레이어가 몬스터를 바라보는 방향의 벡터를 구함
	_vec3 PlayerLook = m_PlayerPos - vPos;

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

//몬스터를 공격한 애랑 몬스터의 각도를 구해주는 함수
_float CMonsterBase::Calc_AngleToAttacker(_vec3 vLook, _vec3 vPos, _vec3 vAttackerPos)
{
	//공격자가 몬스터를 바라보는 방향의 벡터를 구함
	_vec3 AttackerLook = vAttackerPos - vPos;

	//정규화 해주고
	D3DXVec3Normalize(&AttackerLook, &AttackerLook);
	D3DXVec3Normalize(&vLook, &vLook);

	float angleLook = atan2f(vLook.z, vLook.x);
	float angleToAttacker = atan2f(AttackerLook.z, AttackerLook.x);

	float fAngle = D3DXToDegree(angleToAttacker - angleLook);

	if (fAngle > 180.f)
		fAngle -= 360.f;
	if (fAngle < -180.f)
		fAngle += 360.f;

	return fAngle;
}

//플레이어와 몬스터의 각도를 이용해서 몬스터가 보여지는 애니메이션 상태를 정해줌
_int CMonsterBase::Calc_AnimState_byPlayer(_vec3 vLook, _vec3 vPos)
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

//몬스터를 공격한 애랑 몬스터의 각도를 이용해서 몬스터가 보여지는 애니메이션 상태를 정해줌
_int CMonsterBase::Calc_AnimState_byAttacker(_vec3 vLook, _vec3 vPos, _vec3 vAttackerPos)
{
	_float fAngle = Calc_AngleToAttacker(vLook, vPos, vAttackerPos);

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

void CMonsterBase::Debug_Rectangle(_vec3 _LeftTop, _vec3 _RightTop, _vec3 _RightBottom, _vec3 _LeftBottom)
{
	_LeftTop.y -= 2.1f;
	_RightTop.y -= 2.1f;
	_RightBottom.y -= 2.1f;
	_LeftBottom.y -= 2.1f;

	VTXCOL pVertex[6] = {
		{ _LeftTop,				D3DCOLOR_ARGB(0,   255,  64,  64) },
		{ _RightTop,				D3DCOLOR_ARGB(0,   255,  64,  64) },
		{ _RightBottom,		D3DCOLOR_ARGB(200, 255,   0,   0) },

		{ _LeftTop,				D3DCOLOR_ARGB(0,   255,  64,  64) },
		{ _RightBottom,		D3DCOLOR_ARGB(200, 255,   0,   0) },
		{ _LeftBottom,			D3DCOLOR_ARGB(200, 255,   0,   0) },
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

	m_pGraphicDev->SetTexture(0, NULL);																					//
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);								//
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);					//
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			//

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, pVertex, sizeof(VTXCOL));

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);								//
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

void CMonsterBase::Bill_Board()
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

void CMonsterBase::Drop_Item()
{

}

HRESULT CMonsterBase::Add_Component()
{
	CComponent* pComponent = NULL;

	//Buffer
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

	//Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	//Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	//Animation
	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CMonsterBase::Free()
{
	Engine::CCharacter::Free();
}