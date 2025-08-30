#include "pch.h"
#include "CVerticalMovingObject.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CPrototypeManager.h"
#include "CCollisionManager.h"
#include "CTerrainSector.h"
#include "Define.h"


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

	// 기둥처럼 보이게: 얇은 단면(2 x 2), 높이 4.0
	SetVisualScale(2.f, 0.1f, 2.f);

	// 기본 이동 범위(현재 y 기준으로 ±2)
	_vec3 p; m_pTransformCom->Get_Pos(&p);

	// 탑면 Y = 중심Y + m_halfHeight 이고, m_halfHeight는 스케일의 절반 높이를 반영

	// 중심 Y
	float groundY = 0.f; // 임시

	// "탑면이 groundY까지 내려오는" 중심 최소값
	m_minY = groundY - m_halfHeight;

	// 위쪽 끝점은 필요에 맞게 설정 (예: +10만큼 위)
	m_maxY = m_minY + 10.f;

	m_speed = 2.f;
	m_waitAtEnds = 0.9f;
	m_waitTimer = 0.f;
	m_dirUp = true;

	// 시작 y를 범위로 스냅하고 기록
	p.y = std::clamp(p.y, m_minY, m_maxY);
	m_pTransformCom->Set_Pos(p.x, p.y, p.z);

	m_pColliderCom->Set_Owner(this);

	m_lastY = p.y;

	m_pColliderCom->Set_ColLayer(COL_OBJECT);
	m_pColliderCom->Set_ColMask(COL_PLAYER | COL_ENEMY);
	m_pColliderCom->Set_Active(true);

#ifdef _DEBUG
	DebugPrint_ColLayer("Ready");
#endif

	return S_OK;
}

_int CVerticalMovingObject::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	_vec3 pos; m_pTransformCom->Get_Pos(&pos);

	// 1) 끝점 대기
	if (m_waitTimer > 0.f) {
		m_waitTimer -= fTimeDelta;
		if (m_waitTimer < 0.f) m_waitTimer = 0.f;
	}
	else {
		// TODO: 지형 높이를 샘플링해서 groundY에 넣으세요.
		const float groundY = 0.f;

		if (m_dirUp) {
			// 상승: 중심 Y 기준
			pos.y += m_speed * fTimeDelta;
			if (pos.y >= m_maxY) {
				pos.y = m_maxY;
				m_dirUp = false;
				m_waitTimer = m_waitAtEnds;
			}
			m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);
		}
		else {
			// 하강: 탑면이 땅에 닿았는가로 판정
			pos.y -= m_speed * fTimeDelta;
			m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);
			const float groundY = 0.f;   // 필요하면 지형 샘플 값으로 대체
			const float EPS = 0.01f; // z-fighting 방지용 살짝 띄우기

			// 탑면 Y = 중심Y + m_halfHeight (둠식 기둥: 회전 없음 가정)
			if (pos.y + m_halfHeight <= groundY + EPS)
			{
				// 탑면을 groundY+EPS로 '정확히' 스냅
				pos.y = (groundY + EPS) - m_halfHeight;
				m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);
				m_lastY = pos.y;

				m_dirUp = true;            // 반전
				m_waitTimer = m_waitAtEnds;   // 끝점 대기
			}
		}
	}

	// 2) 라이더(플레이어) 동승
 // 동승 처리
	_vec3 cur; m_pTransformCom->Get_Pos(&cur);
	const float deltaY = cur.y - m_lastY;
	m_lastY = cur.y;

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;

	// 현재 rider가 있으면 deltaY만큼 동승
	if (m_pRiderTr && fabsf(deltaY) > 1e-6f) {
		MoveRiderByDeltaY(deltaY, minY);
		// rider가 있으면 홀드 타이머도 계속 리필되고 있음(OnCollision에서)
	}

	// rider가 비어도 홀드 타임 동안은 최소 높이 유지(깜빡임 방지)
	if (!m_pRiderTr && m_groundHoldT > 0.f) {
		m_groundHoldT -= fTimeDelta;
		// 여기서 “최근에 서 있던 대상” Transform을 따로 캐시해두면 좋다.
		// 간단히: 마지막 Transform을 캐시하고 DetachRider에서 nullptr로 만들지 말고 유지하는 방법도 있음.
		// 원한다면 m_pLastRiderTr를 하나 더 두고 그걸로 ClampRiderMinY(minY) 호출.
	}

	// 3) 렌더 큐
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	return 0;
}

void CVerticalMovingObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (m_pColliderCom) {
		m_pColliderCom->Late_Update();
	}
}

void CVerticalMovingObject::Render_GameObject(const _float& fTimeDelta)
{
	if (!m_pBufferCom || !m_pTransformCom) return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	if (m_pTextureCom)
		m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	if (m_pColliderCom)
	{
		DWORD oldZEnable, oldZWrite, oldAlpha;
		m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
		m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWrite);
		m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlpha);

		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pColliderCom->Render(true);   // ★ 항상 위에 보임

		// 원복
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, oldZEnable);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, oldZWrite);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlpha);
	}
}

void CVerticalMovingObject::OnCollision(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	if ((_pOther->Get_ColLayer() & m_pColliderCom->Get_ColMask()) == 0) return;

	// 접촉 유지 중 – rider 갱신 및 최소 높이 유지
	if (_pOther->Get_Owner() != m_pRider) AttachRider(_pOther->Get_Owner());

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;

	ClampRiderMinY(minY);
	m_groundHoldT = m_groundHoldSec; // 매 프레임 갱신
}

void CVerticalMovingObject::OnCollisionEnter(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	// 레이어/마스크 필터 – 플레이어/적 등 발판에 서길 원하는 레이어만
	if ((_pOther->Get_ColLayer() & m_pColliderCom->Get_ColMask()) == 0) return;

	AttachRider(_pOther->Get_Owner());

	const float EPS = 0.02f;
	const float topY = GetTopFaceWorldY();
	const float minY = topY + EPS + m_riderEyeGap;

	// 첫 프레임에 관통 방지만 살짝
	ClampRiderMinY(minY);

	// 접촉 중이므로 홀드 타이머 리필
	m_groundHoldT = m_groundHoldSec;
}

void CVerticalMovingObject::OnCollisionExit(CCollider* _pOther)
{
	if (!_pOther || !m_pColliderCom) return;
	if (_pOther->Get_Owner() == m_pRider) {
		// 바로 Detach하지 않고 Transform 포인터만 유지해도 되지만
		// 여기선 포인터 해제하되 홀드 타이머로 잠깐 유지
		DetachRider();
		// m_groundHoldT는 그대로 – Update에서 줄이며 유지
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

void CVerticalMovingObject::AttachRider(CGameObject* obj)
{
	m_pRider = obj;
	m_pRiderTr = nullptr;
	if (m_pRider) {
		m_pRiderTr = dynamic_cast<Engine::CTransform*>(
			m_pRider->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	}
}

void CVerticalMovingObject::DetachRider()
{
	m_pRider = nullptr;
	m_pRiderTr = nullptr;
}

void CVerticalMovingObject::ClampRiderMinY(float minY)
{
	if (!m_pRiderTr) return;
	_vec3 rp; m_pRiderTr->Get_Pos(&rp);
	if (rp.y < minY) {
		rp.y = minY;
		m_pRiderTr->Set_Pos(rp.x, rp.y, rp.z);
	}
}

void CVerticalMovingObject::MoveRiderByDeltaY(float deltaY, float minY)
{
	if (!m_pRiderTr) return;
	_vec3 rp; m_pRiderTr->Get_Pos(&rp);
	rp.y += deltaY;
	if (rp.y < minY) rp.y = minY;
	m_pRiderTr->Set_Pos(rp.x, rp.y, rp.z);
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