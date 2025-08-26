#include "pch.h"
#include "CBarrelExplosive.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CMonsterBase.h"
#include "CTerrainSector.h"

CBarrelExplosive::CBarrelExplosive(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_iHP(0), m_iFrame_Barrel(0)
{
}

CBarrelExplosive::CBarrelExplosive(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CBarrelExplosive::~CBarrelExplosive()
{
}

HRESULT CBarrelExplosive::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// HP/애니 초기화
	m_iHP = 30;       // 원하는 체력으로 조정
	m_iFrame_Barrel = 0;
	m_iFrame_Explosion = 0;
	m_animAcc = 0.f;
	m_state = State::Idle;
	m_damageApplied = false;

	// 도화선 초기화
	m_isPrimed = false;
	m_fuseAcc = 0.f;
	m_fuseTime = 0.f;

	m_pTransformCom->Set_Pos(20.f, 0.f, 25.f);
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.5f, 1.f));

	// Collider 세팅
	if (m_pColliderCom)
	{
		m_pColliderCom->Ready_CCollider(
			this,
			_vec3(0, 0, 0),         // 초기 위치(Transform가 실제로 잡아줌)
			_vec3(2.f, 3.f, 2.f),   // 박스 크기
			_vec3(0.f, 0.f, 0.f),      // 약간 위로
			COL_OBJECT,             // 내 레이어
			// 공격으로 인정할 대상 레이어(레이 사격 포함)
			COL_BULLET | COL_ATTACK | COL_EXPLOSION | COL_PLAYER
		);
		m_pColliderCom->Set_Active(true);
	}
	// ★ 프레임 수 캐시 (엔진 수정 없음, GetTextureHandle로 갯수 세기)
	m_idleFrameCount = 0;
	if (m_pTexIdle) {
		while (m_pTexIdle->GetTextureHandle(m_idleFrameCount))
			++m_idleFrameCount;
	}

	m_explFrameCount = 0;
	if (m_pTexExplode) {
		while (m_pTexExplode->GetTextureHandle(m_explFrameCount))
			++m_explFrameCount;
	}

	return S_OK;
}

_int CBarrelExplosive::Update_GameObject(const float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);
	Set_OnTerrain();
	Bill_Board();

	// 1, 속도 통합(수평만)
	if (fabsf(m_vel.x) > 1e-6f || fabsf(m_vel.z) > 1e-6f)
	{
		_vec3 pos; m_pTransformCom->Get_Pos(&pos);
		pos.x += m_vel.x * fTimeDelta;
		pos.z += m_vel.z * fTimeDelta;
		m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);

		// 2. 감쇠(마찰) - 지수감쇠 근사
		float damp = max(0.f, 1.f - m_drag * fTimeDelta);
		m_vel.x *= damp;
		m_vel.z *= damp;
		if (fabsf(m_vel.x) < 0.01f) m_vel.x = 0.f;
		if (fabsf(m_vel.z) < 0.01f) m_vel.z = 0.f;
	}

	if (m_state == State::Primed) {
		// 지연 타이머 진행
		m_fuseAcc += fTimeDelta;
		if (m_fuseAcc >= m_fuseTime) {
			BeginExplosion(); // 지연이 끝나면 실제 폭발
		}
	}

	// === 상태 기반 업데이트 ===
	if (m_state == State::Exploding)
	{
		m_animAcc += fTimeDelta;
		const float frameTime = 0.2f;
		if (m_animAcc >= frameTime) {
			m_animAcc = 0.f;

			// 다음 프레임이 없으면 종료
			if (m_explFrameCount <= 0 || (m_iFrame_Explosion + 1) >= m_explFrameCount) {
				m_state = State::Dead;
				m_bDead = true;
				return -1; // DEAD 상수 사용 중이면 그걸로
			}

			++m_iFrame_Explosion;
		}
	}
	else // Idle
	{
		// idle 프레임이 1장 이하이면 0 고정, 2장이면 토글 등
		if (m_idleFrameCount > 1)
			m_iFrame_Barrel = (m_iFrame_Barrel + 1) % m_idleFrameCount;
		else
			m_iFrame_Barrel = 0;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}

void CBarrelExplosive::LateUpdate_GameObject(const float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (m_pColliderCom)
		m_pColliderCom->Late_Update(); // ★ 중요
}

void CBarrelExplosive::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (m_state == State::Exploding) {
		if (m_pTexExplode &&
			m_iFrame_Explosion >= 0 &&
			m_iFrame_Explosion < m_explFrameCount &&
			m_pTexExplode->GetTextureHandle(m_iFrame_Explosion))
		{
			m_pTexExplode->Set_Texture(m_iFrame_Explosion);
		}
	}
	else {
		if (m_pTexIdle &&
			m_iFrame_Barrel >= 0 &&
			(m_idleFrameCount == 0 || m_iFrame_Barrel < m_idleFrameCount) &&
			m_pTexIdle->GetTextureHandle(m_iFrame_Barrel))
		{
			m_pTexIdle->Set_Texture(m_iFrame_Barrel);
		}
	}

	m_pBufferCom->Render_Buffer();

	//충돌체
	m_pColliderCom->Render(g_colRend);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBarrelExplosive::OnCollision(CCollider* _pOther)
{
}

void CBarrelExplosive::OnCollisionEnter(CCollider* _pOther)
{
	char buf[128];

	if (!_pOther) return;

	//sprintf_s(buf, "[Barrel] OnCollisionEnter layer=0x%08X\n",
	//	(unsigned)_pOther->Get_ColLayer());
	//OutputDebugStringA(buf);

	// 폭발 중에는 추가 히트 무시
	if (m_state != State::Idle) return;

	const auto layer = _pOther->Get_ColLayer();

	if (layer & COL_BULLET)
	{
		sprintf_s(buf, "[Barrel] OnCollisionEnter layer=0x%08X\n",
			(unsigned)_pOther->Get_ColLayer());
		OutputDebugStringA(buf);
		TakeDamage(10);
	}
	return;


	// 체인 폭발
	if (layer & COL_EXPLOSION)
	{
		TakeDamage(9999);
		return;
	}

	return;
}

void CBarrelExplosive::OnCollisionExit(CCollider* _pOther)
{
}

void CBarrelExplosive::TakeDamage(int dmg)
{
	if (m_state != State::Idle) return; // 이미 터지는 중이면 무시
	m_iHP -= dmg;
	if (m_iHP <= 0) BeginExplosion();
}

void CBarrelExplosive::BeginExplosion()
{
	m_state = State::Exploding;
	m_animAcc = 0.f;
	m_iFrame_Explosion = 0;

	// 더 이상 피격/겹침 없도록 콜라이더 비활성화
	if (m_pColliderCom) m_pColliderCom->Set_Active(false);

	// 첫 프레임에 반경 데미지 1회
	ApplyExplosionDamage();

	// 폭발 스프라이트가 아예 없거나 1장도 없으면 바로 제거
	if (m_explFrameCount <= 0) {
		m_state = State::Dead;
		m_bDead = true;
	}
}

void CBarrelExplosive::ApplyExplosionDamage()
{
	if (m_damageApplied) return;
	m_damageApplied = true;

	// 내 위치
	_vec3 myPos; m_pTransformCom->Get_Pos(&myPos);

	const float R = 4.0f;        // 폭발 반경(필요시 4~6 사이로 조정)
	const float R2 = R * R;

	Engine::CScene* pScene = Engine::CManagement::GetInstance()->Get_Scene();
	if (!pScene) return;

	// 1) 몬스터에 피해
	{
		const auto& monsters = pScene->Get_TotalGameObject(OBJID::OBJ_ENEMY);
		for (auto* obj : monsters)
		{
			if (!obj || obj->Get_Dead()) continue;

			auto* tr = dynamic_cast<Engine::CTransform*>(obj->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
			if (!tr) continue;

			_vec3 pos; tr->Get_Pos(&pos);
			_vec3 d = pos - myPos;
			_float dist2 = D3DXVec3LengthSq(&d);
			if (dist2 <= R2)
			{
				if (auto* pMonster = dynamic_cast<CMonsterBase*>(obj))
				{
					_float dist = sqrtf(dist2);

					// 거리에 따라 데미지 감쇠
					_float t = max(0.f, 1.f - dist / R);	// 0 ~ 1
					_int damage = max(1, int(100.f * t));	// 최대 데미지 100
					pMonster->Take_Dmg(damage);
				}
			}
		}
	}

	// 1.5) 플레이어 피해 + 넉백
	{
		auto* pPlayer = Engine::CManagement::GetInstance()->Get_Player();
		if (pPlayer && !pPlayer->Get_Dead())
		{
			auto* pPlTr = dynamic_cast<Engine::CTransform*>(
				pPlayer->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
			if (pPlTr)
			{
				_vec3 pPos; pPlTr->Get_Pos(&pPos);
				_vec3 d = pPos - myPos; d.y = 0.f;

				float dist2 = D3DXVec3LengthSq(&d);
				if (dist2 <= R2)
				{
					float dist = sqrtf(dist2);
					// 거리 감쇠(중심에서 강, 가장자리 약)
					float t = max(0.f, 1.f - dist / R);     // 0..1
					int   dmg = max(1, int(80.f * t));      // 폭발 데미지(원하는 수치로)

					// 플레이어에게 적용 (아래 B에서 구현)
					if (auto* cam = dynamic_cast<CDynamicCamera*>(pPlayer))
					{
						cam->Take_Damage(dmg);              // 실제 플레이어 데미지
					}
				}
			}
		}
	}

	// 2) 다른 배럴에도 피해 → 연쇄 폭발 유발
	{
		const auto& objects = pScene->Get_TotalGameObject(OBJID::OBJ_OBJECT);
		for (auto* obj : objects)
		{
			if (!obj || obj == this || obj->Get_Dead()) continue;

			auto* other = dynamic_cast<CBarrelExplosive*>(obj);
			if (!other) continue;

			auto* tr = dynamic_cast<Engine::CTransform*>(
				obj->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
			if (!tr) continue;

			_vec3 pos; tr->Get_Pos(&pos);
			_vec3 d = pos - myPos;
			float dist2 = D3DXVec3LengthSq(&d);
			if (dist2 > R2) continue;

			// 거리 기반 지연으로 ‘웨이브’ 연출
			float dist = sqrtf(dist2);
			float t = max(0.f, 1.f - dist / R);     // 0..1
			float impulse = m_pushForce * (0.4f + 0.6f * t); // 너무 약해지지 않게 바닥값 0.4 배

			// ★ 먼저 '밀어내기'
			other->AddImpulseXZ(d, impulse);

			// ★ 그리고 '점화 지연'으로 연쇄 폭발
			float baseDelay = 0.12f;
			float perMeter = 0.06f;
			float delay = baseDelay + perMeter * dist;
			other->PrimeFuse(delay);            // 바로 터뜨리지 않고 ‘점화’만!
		}
	}
}

void CBarrelExplosive::PrimeFuse(float delaySec)
{
	if (m_state != State::Idle) return; // 이미 폭발 중/사망/점화면 무시
	if (m_isPrimed) return;             // 중복 점화 방지

	m_isPrimed = true;
	m_state = State::Primed;
	m_fuseAcc = 0.f;
	m_fuseTime = delaySec;              // 지연 시간(초)
}

void CBarrelExplosive::AddImpulseXZ(const _vec3& dir, float magnitude)
{
	_vec3 n = dir;  n.y = 0.f;
	if (D3DXVec3LengthSq(&n) < 1e-6f) return;
	D3DXVec3Normalize(&n, &n);

	m_vel.x += n.x * magnitude;
	m_vel.z += n.z * magnitude;
}

HRESULT CBarrelExplosive::Add_Component()
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

	//Texture
	// ★ Idle 텍스처
	m_pTexIdle = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Barrel"));
	if (!m_pTexIdle) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_TexIdle", m_pTexIdle });

	// ★ 폭발 텍스처
	m_pTexExplode = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BarrelExplosion"));
	if (!m_pTexExplode) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_TexExpl", m_pTexExplode });



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

	OutputDebugStringA("[Stage] add OBJ_ITEM: Barrel_Explosion\n");

	return S_OK;
}

void CBarrelExplosive::Set_OnTerrain()
{
	for (auto terrain : CManagement::GetInstance()->Get_Scene()->Get_Terrain())
	{
		_vec3 pos;
		m_pTransformCom->Get_Pos(&pos);

		auto dterrain = dynamic_cast<CTerrainSector*>(terrain);
		auto bufferCom = dterrain->Get_BufferCom();
		auto transformCom = dterrain->Get_Transform();

		_float fScaleY = m_pTransformCom->Get_Scale()->y;

		_vec3 onPosition{ 0.f, 0.f, 0.f };
		if (m_pCalculatorCom->RayScan_OnTerrain(bufferCom, transformCom, &onPosition, &pos))
		{
			m_pTransformCom->Set_Pos(pos.x, onPosition.y + fScaleY, pos.z);
			break;
		}
	}
}

void CBarrelExplosive::Bill_Board()
{
	if (!m_pTransformCom)
		return;

	_matrix matWorld, matView, matBill;
	m_pTransformCom->Get_World(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);		// 카메라가 갖고 있는 y회전의 역행렬 -역행렬> 카메라의 월드 상태의 y축 회전 행렬

	matWorld = matBill * matWorld;
	// 최종적으로 빌보드 행렬을 월드 행렬로 설정                                           
	m_pTransformCom->Set_World(&matWorld);
}

CBarrelExplosive* CBarrelExplosive::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBarrelExplosive* pBarrel = new CBarrelExplosive(pGraphicDev);

	return pBarrel;
}

CBarrelExplosive* CBarrelExplosive::Clone()
{
	CBarrelExplosive* pBE = new CBarrelExplosive(*this);

	if (FAILED(pBE->Ready_GameObject()))
	{
		Safe_Release(pBE);
		MSG_BOX("Barrel Create Failed");
		return nullptr;
	}

	return pBE;
}

void CBarrelExplosive::Free()
{
	Engine::CGameObject::Free();
}
