#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

static const char* ColLayerToStr(int l) {
	switch (l) {
	case COL_PLAYER: return "COL_PLAYER";
	case COL_OBJECT: return "COL_OBJECT";
		// 필요하면 다른 레이어도 추가
	default:         return "UNKNOWN";
	}
}

class CVerticalMovingObject : public CGameObject
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

public:
	// CCubeTex 단위와 피벗 지정
	void SetMeshUnitAndPivot(float unit, bool pivotIsBottom)
	{
		m_meshUnit = unit;
		m_pivotIsBottom = pivotIsBottom;

		// 현재 스케일로 즉시 재동기화
		if (m_pTransformCom) {
			const _vec3* s = m_pTransformCom->Get_Scale();
			if (s) SyncColliderToVisual(s->x, s->y, s->z);
		}
	}

	void SetRiderEyeGap(float gap) { m_riderEyeGap = max(0.f, gap); }

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

	float m_riderEyeGap = 0.18f; // 발판 위에서 카메라를 바닥에서 이만큼 띄움(미세 조절용)

	float m_meshUnit = 2.f;				// CCubeTex가 한 변 2이면 2.f
	bool  m_pivotIsBottom = false;		// 메시 피벗이 바닥이면 true

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
			"[Pillar] %s layer=%s(%d) mask=0x%X active=%d\n",
			tag ? tag : "",
			ColLayerToStr(layer), layer,
			mask,
			(int)act);

		OutputDebugStringA(buf);
	}

	void SyncColliderToVisual(float sx, float sy, float sz)
	{
		if (!m_pColliderCom || !m_pTransformCom) return;

		// 메시 단위 보정: CCubeTex 기본이 한 변 2라면 m_meshUnit=2.0f
		_vec3 colScale = _vec3(sx * m_meshUnit, sy * m_meshUnit, sz * m_meshUnit);

		// 피벗이 바닥이면 콜라이더를 위로 halfHeight만큼 올려 맞춤
		_vec3 offset = m_pivotIsBottom ? _vec3(0.f, colScale.y * 0.5f, 0.f) : _vec3(0, 0, 0);

		_vec3 pos; m_pTransformCom->Get_Pos(&pos);

		// 내부 상태 세팅(라인 생성 포함) + 확정 값 못박기
		m_pColliderCom->Ready_CCollider(this, pos, colScale, offset, COL_OBJECT, (COL_PLAYER));
		m_pColliderCom->Set_ColLayer(COL_OBJECT);
		m_pColliderCom->Set_ColMask(COL_PLAYER);
		m_pColliderCom->Set_Active(true);
		m_pColliderCom->Set_Scale(colScale);
		// Set_Offset/Set_OffsetPos 가 있으면 offset도 함께 반영

		// 탑면 계산 일관성 유지
		m_halfHeight = colScale.y * 0.5f;
	}

	// 라이더(플랫폼 위 대상) 포인터들 – Transform만 사용
	Engine::CTransform* m_pRiderTr = nullptr;

	CGameObject* m_pRiderOwner = nullptr;

	// 최근 라이더(홀드 동안 사용) + 포스트 캐리 값
	Engine::CTransform* m_pLastRiderTr = nullptr;
	float m_postCarryDeltaY = 0.f;  // 이번 프레임 발판이 움직인 y 변화량
	float m_postCarryMinY = 0.f;  // 발판 상면 + 여유(EPS + gap)

	// 발판 상면과의 최소 간격/지연 해제
	float m_groundHoldSec = 0.08f;    // 접촉 끊겨도 유지할 시간
	float m_groundHoldT = 0.0f;     // 남은 홀드 타이머

	// === Transform만 뽑는 헬퍼 ===
	Engine::CTransform* TryGetTransform(Engine::CGameObject* obj) const;

	// 라이더 캡처/해제 헬퍼
	void AttachRider(CGameObject* obj);
	void DetachRider();

	// 라이더 Y 보정/동승
	void ClampRiderMinY(Engine::CTransform* tr, float minY);           // 최소 높이 보장(관통방지)
	void MoveRiderByDeltaY(Engine::CTransform* tr, float deltaY, float minY); // deltaY 동승 + 최소 높이

	static float s_activeGroundY;
	static float s_activeGroundTTL;

	struct SActiveGround {
		float y = -FLT_MAX; // 바닥 y (없으면 -INF)
		float cx = 0.f, cz = 0.f; // 중심 (x,z)
		float hx = 0.f, hz = 0.f; // 반폭 (x,z)
		float ttl = 0.f;           // 유지 시간
	};
	static SActiveGround s_ag;      // 모든 발판이 공유해 누적 게시(필요시 가장 최근/가장 높은 값 사용)

public:
	// ★ 발판이 이번 프레임 "최소 바닥 높이"를 게시/조회하는 정적 API
	static void  PublishActiveGroundY(float y);
	static float GetActiveGroundY();
	static void  DecayActiveGround(float fTimeDelta);

	// 발판 상면 바닥 높이 게시/조회 (영역 포함)
	static void  PublishActiveGroundArea(float y, float cx, float cz, float hx, float hz);
	static bool  QueryActiveGroundYAt(float x, float z, float& outY);

public:
	static CVerticalMovingObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CVerticalMovingObject* Clone() override;

private:
	virtual void Free() override;
};

