#include "pch.h"
#include "CVerticalMovingObject.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CPrototypeManager.h"
#include "CCollisionManager.h"
#include "CTerrainSector.h"
#include "Define.h"
#include "CSoundManager.h"

CVerticalMovingObject::CVerticalMovingObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_pBufferCom(nullptr), m_pTextureCom(nullptr), m_pCalculatorCom(nullptr),
	m_pRider(nullptr)
{
}

CVerticalMovingObject::CVerticalMovingObject(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CVerticalMovingObject::~CVerticalMovingObject()
{
}

HRESULT CVerticalMovingObject::Ready_GameObject()
{
	OutputDebugStringA("[Pillar] Ready()\n");

	if (FAILED(Add_Component())) return E_FAIL;

	// 기둥처럼 보이게: 얇은 단면(4 x 4), 높이 0.4
	SetVisualScale(4.f, 0.5f, 4.f);

	// 기본 이동 범위(현재 y 기준으로 ±2)
	_vec3 p; m_pTransformCom->Get_Pos(&p);

	// 탑면 Y = 중심Y + m_halfHeight 이고, m_halfHeight는 스케일의 절반 높이를 반영

	// 중심 Y
	//float groundY = 0.f; // 임시

	// "탑면이 groundY까지 내려오는" 중심 최소값
	m_minY = p.y;

	// 위쪽 끝점은 필요에 맞게 설정 (예: +10만큼 위)
	m_maxY = m_minY + 6.3f;

	m_speed = 2.f;
	m_waitAtEnds = 0.9f;
	m_waitTimer = 0.f;
	m_dirUp = false;

	// 시작 y를 범위로 스냅하고 기록
	p.y = std::clamp(p.y, m_minY, m_maxY);
	m_pTransformCom->Set_Pos(p.x, p.y, p.z);

	m_pColliderCom->Set_Owner(this);

	m_lastY = p.y;

	m_pColliderCom->Set_ColLayer(COL_OBJECT);
	m_pColliderCom->Set_ColMask(COL_PLAYER);
	m_pColliderCom->Set_Active(true);

#ifdef _DEBUG
	DebugPrint_ColLayer("Ready");
#endif

	return S_OK;
}

_int CVerticalMovingObject::Update_GameObject(const _float& fTimeDelta)
{
	const _int iExit = CGameObject::Update_GameObject(fTimeDelta);

	const bool hasRider = (m_pRiderTr != nullptr) || (m_pLastRiderTr && m_groundHoldT > 0.f);
	if (hasRider)
	{
		_vec3 pos; m_pTransformCom->Get_Pos(&pos);
		if (m_waitTimer > 0.f) {
			m_waitTimer -= fTimeDelta;
			if (m_waitTimer < 0.f) m_waitTimer = 0.f;
		}
		else {
			if (m_dirUp) {
				pos.y += m_speed * fTimeDelta;
				if (pos.y >= m_maxY) {
					pos.y = m_maxY;
					m_dirUp = false;
					m_waitTimer = m_waitAtEnds;
				}
			}
			else {
				pos.y -= m_speed * fTimeDelta;
				if (pos.y <= m_minY) {
					pos.y = m_minY;
					m_dirUp = true;
					m_waitTimer = m_waitAtEnds;
				}
			}
			m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);
		}

		// 라이더 동승 계산
		_vec3 cur; m_pTransformCom->Get_Pos(&cur);
		const float deltaY = cur.y - m_lastY;
		m_postCarryDeltaY = deltaY;
		m_lastY = cur.y;

		const float EPS = 0.02f;
		m_postCarryMinY = (cur.y + m_halfHeight) + EPS + m_riderEyeGap;
	}

	// 렌더 등록은 항상
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	return iExit;
}

void CVerticalMovingObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	
	const bool hasRider = (m_pRiderTr != nullptr) || (m_pLastRiderTr && m_groundHoldT > 0.f);
	if (hasRider)
	{
		if (m_pRiderTr) {
			if (fabsf(m_postCarryDeltaY) > 1e-6f)
				MoveRiderByDeltaY(m_pRiderTr, m_postCarryDeltaY, m_postCarryMinY);
			else
				ClampRiderMinY(m_pRiderTr, m_postCarryMinY);
		}

		if (m_pRiderTr || (m_pLastRiderTr && m_groundHoldT > 0.f)) {
			_vec3 p; m_pTransformCom->Get_Pos(&p);
			const _vec3* s = m_pTransformCom->Get_Scale();
			const float hx = s->x * 0.5f;
			const float hz = s->z * 0.5f;
			PublishActiveGroundArea(m_postCarryMinY, p.x, p.z, hx, hz);
		}

		DecayActiveGround(fTimeDelta);

		
	}
	if (m_pColliderCom) m_pColliderCom->Late_Update();
}

void CVerticalMovingObject::Render_GameObject(const _float& fTimeDelta)
{
	if (!m_pBufferCom || !m_pTransformCom) return;

	// 상태 저장
	DWORD oldAlphaTest, oldAlphaBlend, oldZWrite, oldZEnable, oldCull;
	m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &oldAlphaTest);
	m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlend);
	m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWrite);
	m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
	m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &oldCull);

	// 정상값 강제
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	if (m_pTextureCom) m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

#ifdef _DEBUG
	if (m_pColliderCom) m_pColliderCom->Render(g_colRend);
#endif

	// 상태 복구
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, oldAlphaTest);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlend);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, oldZWrite);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, oldZEnable);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, oldCull);
}

void CVerticalMovingObject::OnCollision(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	if ((_pOther->Get_ColLayer() & COL_PLAYER) == 0) return;

	if (!m_pRiderTr) AttachRider(_pOther->Get_Owner());
#ifdef _DEBUG
	if (!m_pRiderTr) {
		char buf[128];
		sprintf_s(buf, "[Platform] NO Com_Transform on %p (Stay)\n", _pOther->Get_Owner());
		OutputDebugStringA(buf);
	}
	else {
		OutputDebugStringA("[Platform] rider Transform OK (Stay)\n");
	}
#endif

	m_groundHoldT = m_groundHoldSec;    // 접촉 유지 → 홀드 리필

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;
	ClampRiderMinY(m_pRiderTr, minY);
}

void CVerticalMovingObject::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	// 레이어/마스크 필터 – 플레이어/적 등 발판에 서길 원하는 레이어만
	if ((_pOther->Get_ColLayer() & COL_PLAYER) == 0) return;

	AttachRider(_pOther->Get_Owner());

	_vec3 pos; m_pTransformCom->Get_Pos(&pos);
	CSoundManager::GetInstance()->PlaySound3D(L"MOVING", pos, 0.5f);

#ifdef _DEBUG
	if (!m_pRiderTr) {
		char buf[128];
		sprintf_s(buf, "[Platform] NO Com_Transform on %p (Enter)\n", _pOther->Get_Owner());
		OutputDebugStringA(buf);
	}
	else {
		OutputDebugStringA("[Platform] rider Transform OK (Enter)\n");
	}
#endif

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;

	// 첫 프레임에 관통 방지만 살짝
	ClampRiderMinY(m_pRiderTr, minY);
}

void CVerticalMovingObject::OnCollisionExit(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	if (m_pRiderTr && _pOther->Get_Owner()
		&& TryGetTransform(_pOther->Get_Owner()) == m_pRiderTr)
	{
		DetachRider();
		// m_groundHoldT는 Update/LateUpdate에서 감소
	}
}

HRESULT CVerticalMovingObject::Add_Component()
{
	CComponent* pComponent = nullptr;

	// 1) Buffer: 플랫폼 시각화(평면). 필요에 따라 CCubeTex로 바꿔도 됨.
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_CubeTex"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// 2) Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// 3) Texture (선택): 프로젝트에 맞는 텍스처 프로토타입이 있으면 사용
	// 없으면 nullptr로 두고 버퍼만 그려도 됨.
	if (auto* tex = dynamic_cast<Engine::CTexture*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Elevator_Floor")))
	{
		m_pTextureCom = tex;
		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", tex });
	}

	// 4) Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	// 5) Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>(
		CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CVerticalMovingObject::CarryRiderByDeltaY(float deltaY)
{
	if (!m_pRider) return;

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;		// 최소 높이

	// (폴백) Transform 보유 객체
	if (auto* tr = dynamic_cast<Engine::CTransform*>(
		m_pRider->Get_Component(ID_DYNAMIC, L"Com_Transform"))) {
		_vec3 rp; tr->Get_Pos(&rp);
		rp.y += deltaY;
		if (rp.y < minY) rp.y = minY;
		tr->Set_Pos(rp.x, rp.y, rp.z);
	}

}

bool CVerticalMovingObject::IsPlayerObject(CGameObject* obj) const
{
	if (!obj) return false;

	return false;
}

Engine::CTransform* CVerticalMovingObject::TryGetTransform(Engine::CGameObject* obj) const
{
	if (!obj) return nullptr;

	// 1) 가장 표준: DYNAMIC의 "Com_Transform"
	if (auto* tr = dynamic_cast<Engine::CTransform*>(
		obj->Get_Component(ID_DYNAMIC, L"Com_Transform")))
		return tr;

	// 2) 일부 객체는 STATIC에 둘 수도 있음
	if (auto* tr = dynamic_cast<Engine::CTransform*>(
		obj->Get_Component(ID_STATIC, L"Com_Transform")))
		return tr;

	// 3) (선택) 다른 이름을 쓰는 프로젝트라면 여기서 추가로 시도
	// if (auto* tr = dynamic_cast<Engine::CTransform*>(obj->Get_Component(ID_DYNAMIC, L"Com_PlayerTransform"))) return tr;

	// 4) 최후: 못 얻으면 nullptr
	return nullptr;
}

void CVerticalMovingObject::AttachRider(CGameObject* obj)
{
	if (!obj) return;
	auto* tr = TryGetTransform(obj);
	if (!tr) return;

	// 이미 플레이어를 태우고 있다면 유지
	if (m_pRiderOwner && m_pRiderOwner != obj) return;

	m_pRiderOwner = obj;            // ★ 소유자 기억
	m_pRiderTr = tr;
	m_pLastRiderTr = tr;
	m_groundHoldT = m_groundHoldSec;
}

void CVerticalMovingObject::DetachRider()
{
	// 마지막 라이더 Transform을 홀드 동안 유지
	m_pLastRiderTr = m_pRiderTr;
	m_pRider = nullptr;
	m_pRiderTr = nullptr;
}

void CVerticalMovingObject::ClampRiderMinY(Engine::CTransform* tr, float minY)
{
	if (!tr) return;
	_vec3 p; tr->Get_Pos(&p);
	if (p.y < minY) { p.y = minY; tr->Set_Pos(p.x, p.y, p.z); }
}

void CVerticalMovingObject::MoveRiderByDeltaY(Engine::CTransform* tr, float deltaY, float minY)
{
	if (!tr) return;
	_vec3 p; tr->Get_Pos(&p);
	p.y += deltaY;
	if (p.y < minY) p.y = minY;
	tr->Set_Pos(p.x, p.y, p.z);
}

CVerticalMovingObject::SActiveGround CVerticalMovingObject::s_ag = {};

float CVerticalMovingObject::s_activeGroundY = -FLT_MAX;
float CVerticalMovingObject::s_activeGroundTTL = 0.f;

void CVerticalMovingObject::PublishActiveGroundY(float y)
{
	if (y > s_activeGroundY) s_activeGroundY = y;
	s_activeGroundTTL = 0.12f; // 120ms 유지
}

float CVerticalMovingObject::GetActiveGroundY()
{
	return s_activeGroundY;
}

void CVerticalMovingObject::DecayActiveGround(float fTimeDelta)
{
	if (s_ag.ttl > 0.f) {
		s_ag.ttl -= fTimeDelta;
		if (s_ag.ttl <= 0.f) {
			s_ag = SActiveGround{}; // 리셋
		}
	}
}

void CVerticalMovingObject::PublishActiveGroundArea(float y, float cx, float cz, float hx, float hz)
{
	// 매 프레임 들어오는 최신 값으로 덮어쓰기
	s_ag.y = y;
	s_ag.cx = cx;  s_ag.cz = cz;
	s_ag.hx = hx;  s_ag.hz = hz;
	s_ag.ttl = 0.05f;  // 너무 오래 남지 않게 줄이기(권장: 0.03~0.06)
}

bool CVerticalMovingObject::QueryActiveGroundYAt(float x, float z, float& outY)
{
	if (s_ag.ttl <= 0.f) return false;
	// 영역 안(패딩 약간)
	const float pad = 0.15f;
	if (fabsf(x - s_ag.cx) <= (s_ag.hx + pad) &&
		fabsf(z - s_ag.cz) <= (s_ag.hz + pad)) {
		outY = s_ag.y;
		return true;
	}
	return false;
}

CVerticalMovingObject* CVerticalMovingObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	OutputDebugStringA("[Pillar] Create()\n");

	CVerticalMovingObject* pObj = new CVerticalMovingObject(pGraphicDev);

	return pObj;
}

CVerticalMovingObject* CVerticalMovingObject::Clone()
{
	OutputDebugStringA("[Pillar] Clone()\n");

	CVerticalMovingObject* pObj = new CVerticalMovingObject(*this);
	if (FAILED(pObj->Ready_GameObject())) {
		Safe_Release(pObj);
		MSG_BOX("VerticalMovingObject Create Failed");
		return nullptr;
	}
	return pObj;
}


void CVerticalMovingObject::Free()
{
	Engine::CGameObject::Free();
}