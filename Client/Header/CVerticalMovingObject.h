#pragma once
#include "CGameObject.h"
#include "CDynamicCamera.h"

static const char* ColLayerToStr(int l) {
	switch (l) {
	case COL_PLAYER: return "COL_PLAYER";
	case COL_OBJECT: return "COL_OBJECT";
		// 필요하면 다른 레이어도 추가
	default:         return "UNKNOWN";
	}
}

class CVerticalMovingObject :    public CGameObject
{
private:
	explicit CVerticalMovingObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVerticalMovingObject(const CGameObject& rhs);
	virtual ~CVerticalMovingObject();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	// 설치·튜닝용 파라미터
	void SetMovement(float minY, float maxY, float speed, float waitAtEnds = 0.f)
	{
		m_minY = min(minY, maxY);
		m_maxY = max(minY, maxY);
		m_speed = max(0.0f, speed);
		m_waitAtEnds = max(0.0f, waitAtEnds);
		// 시작 위치를 범위 안으로 스냅
		_vec3 p; m_pTransformCom->Get_Pos(&p);
		p.y = std::clamp(p.y, m_minY, m_maxY);
		m_pTransformCom->Set_Pos(p.x, p.y, p.z);
		m_dirUp = (fabsf(p.y - m_minY) < fabsf(p.y - m_maxY)); // 시작 방향 대충 결정
	}

	void SetVisualScale(float sx, float sy, float sz)
	{
		if (m_pTransformCom) m_pTransformCom->Set_Scale(_vec3(sx, sy, sz));
		// 메시 스케일 변경 시, 콜라이더도 동일 크기로 즉시 동기화
		SyncColliderToVisual(sx, sy, sz);
	}

	// 문이랑 스위치 태그기능
public:
	void Set_LinkId(const wstring& id) { m_linkId = id; }
	const wstring& Get_LinkId() const { return m_linkId; }

private:
	wstring m_linkId;

private:
	bool m_bPressed = false;
	_float m_fTimer = 0.f;

private:
	HRESULT Add_Component();
	void    CarryRiderByDeltaY(float deltaY);
	bool    IsPlayerObject(CGameObject* obj) const;

private:
	Engine::CCubeTex* m_pBufferCom;					// 평면, 플랫폼 그리기
	Engine::CTexture* m_pTextureCom;						// 스위치 초기상태
	Engine::CCalculator* m_pCalculatorCom;

	// 이동 파라미터
	float  m_minY = 0.f;
	float  m_maxY = 0.f;
	float  m_speed = 2.f;
	float  m_waitAtEnds = 0.f;   // 끝점에서 정지 시간(초)
	float  m_waitTimer = 0.f;
	bool   m_dirUp = true;

	// 라이더(플랫폼 위의 플레이어)
	CGameObject* m_pRider = nullptr;
	float        m_lastY = 0.f;     // 이전 프레임 플랫폼 Y (deltaY 계산)
	float        m_halfHeight = 0.5f;// 콜라이더/메시 절반 높이

	float GetTopFaceWorldY() const {
		if (m_pColliderCom) {
			const _vec3* v = m_pColliderCom->Get_WorldCollisionBox(); // 콜라이더 8점
			float top = v[0].y;
			for (int i = 1; i < 8; ++i) if (v[i].y > top) top = v[i].y;
			return top;
		}
		_vec3 p; m_pTransformCom->Get_Pos(&p);
		return p.y + m_halfHeight; // 폴백
	}

	void SnapTopTo(float targetY) {
		_vec3 pos; m_pTransformCom->Get_Pos(&pos);
		float top = GetTopFaceWorldY();
		float off = targetY - top;        // top을 targetY로 맞추도록 보정
		if (fabsf(off) > 1e-5f) {
			pos.y += off;
			m_pTransformCom->Set_Pos(pos.x, pos.y, pos.z);
			m_lastY = pos.y;              // 연속성 유지
		}
	}

	float TopY() const {
		_vec3 p; m_pTransformCom->Get_Pos(&p);
		return p.y + m_halfHeight;       // 회전 없다는 전제(둠식 기둥)
	}

	void SetTopY(float targetY) {
		_vec3 p; m_pTransformCom->Get_Pos(&p);
		p.y = targetY - m_halfHeight;    // 탑면이 targetY가 되도록 중심Y 조정
		m_pTransformCom->Set_Pos(p.x, p.y, p.z);
		m_lastY = p.y;
	}

	void DebugPrint_ColLayer(const char* tag)
	{
		if (!m_pColliderCom) {
			OutputDebugStringA("[Pillar] collider = nullptr\n");
			return;
		}

		const int   layer = (int)m_pColliderCom->Get_ColLayer();
		const UINT  mask = (UINT)m_pColliderCom->Get_ColMask();
		const BOOL  act = (BOOL)m_pColliderCom->Get_Active();

		char buf[256];
		sprintf_s(buf,
			"[Pillar %p] %s layer=%s(%d) mask=0x%X active=%d\n",
			this,
			tag ? tag : "",
			ColLayerToStr(layer), layer,
			mask,
			(int)act);

		OutputDebugStringA(buf);
	}

	void SyncColliderToVisual(float sx, float sy, float sz)
	{
		if (!m_pColliderCom || !m_pTransformCom) return;

		// ── ① 피벗/베이스 크기 보정 ─────────────────────────────
		// CCubeTex가 보통 -0.5~+0.5 범위(=한 변 1)라면 아래 값 1.0f 유지.
		// 만약 -1~+1(=한 변 2)라면 kMeshUnit=2.0f 로 바꿔주세요.
		const float kMeshUnit = 1.0f;   // 필요시 2.0f

		// 피벗이 '중앙'이면 (0,0,0). 피벗이 '바닥'이면 +절반높이로 올려서 맞춤.
		// (디버그 라인이 아래로 반 칸 빠져보이면 아래 false→true 로 바꾸세요)
		const bool  kPivotIsBottom = false;
		_vec3 offset = kPivotIsBottom ? _vec3(0.f, +sy * 0.5f, 0.f) : _vec3(0, 0, 0);

		// ── ② 최종 콜라이더 스케일(=월드에서 실제 크기) ─────────
		_vec3 colScale = _vec3(sx * kMeshUnit, sy * kMeshUnit, sz * kMeshUnit);

		// ── ③ 현재 위치로 재초기화 & 값 못 박기 ────────────────
		_vec3 pos; m_pTransformCom->Get_Pos(&pos);

		// Ready_CCollider가 내부 상태를 세팅하므로, 스케일/오프셋을 여기서 정의
		m_pColliderCom->Ready_CCollider(this, pos, colScale, offset,
			COL_OBJECT, (COL_PLAYER | COL_ENEMY));
		m_pColliderCom->Set_ColLayer(COL_OBJECT);
		m_pColliderCom->Set_ColMask(COL_PLAYER | COL_ENEMY);
		m_pColliderCom->Set_Active(true);
		m_pColliderCom->Set_Scale(colScale);
		// (Set_OffsetPos/Set_Offset 함수가 있으면 offset도 같이 세팅)

		// ── ④ 상단면 계산도 '콜라이더 기준'으로 통일 ────────────
		// 헷갈리지 않게 항상 콜라이더 높이로 halfHeight 갱신
		m_halfHeight = colScale.y * 0.5f;
	}

public:
	static CVerticalMovingObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CVerticalMovingObject* Clone() override;

private:
	virtual void Free() override;
};

