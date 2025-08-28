#pragma once
#include "CCamera.h"
#include "CPrototypeManager.h"

class CDynamicCamera :
	public CCamera
{
private:
	enum PLAYER_STATE { IDLE, HURT1, HURT2, HURT3, HURT4, DEAD };

private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicCamera(const CDynamicCamera& rhs);
	virtual ~CDynamicCamera();

public:
	HRESULT Ready_GameObject(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual         _int        Update_GameObject(const _float& fTimeDelta);
	virtual         void        LateUpdate_GameObject(const _float& fTimeDelta);
	virtual         void        Render_GameObject(const _float& fTimeDelta);

public:
	_vec3           Get_Pos() { return m_vEye; }
	void            Set_Pos(_vec3 vPos) { m_vEye = vPos; }
	_vec3           Get_At() { return m_vAt; }
	_vec3           Get_Up() { return m_vUp; }

	PLAYER_STATE    Get_State() { return m_eState; }
	void            Set_State(PLAYER_STATE pState) { m_eState = pState; }

	_bool           Get_Trigger() { return m_bTrigger; }
	void            Set_Trigger(_bool bTrigger) { m_bTrigger = bTrigger; }

	_int            Get_Hp() { return m_iGameHp; }
	void            Set_Hp(_int i) { m_iGameHp = i; }

	_int            Get_Armor() { return m_iArmor; }
	void            Set_Armor(_int i) { m_iArmor = i; }

	_int            Get_MaxNum() { return m_iMax; }

public:
	void            OnCollision(CCollider* _pOther) override;
	void            OnCollisionEnter(CCollider* _pOther) override;
	void            OnCollisionExit(CCollider* _pOther) override;

public:
	// 08-12 - 최대 체력 넘어가면 안되는 코드 제작(200은 최대 Armor랑 최대 Hp, 최대 Clip수로 일단은 제한)
	// 08-13 - 최대 수치 관련 로직 수정
	// 08-15 - 최대 수치 넘어가지 않게 수정
	void AddHP(_int heal)
	{
		m_iGameHp += heal;
		if (m_iGameHp >= m_iMax)
			m_iGameHp = m_iMax;
	}

	void AddArmor(_int armor)
	{
		m_iArmor += armor;
		if (m_iArmor >= m_iMax)
			m_iArmor = m_iMax;
	}

	void Take_Damage(_int iDamage)
	{
		if (m_iGameHp <= 0)
			m_iGameHp = 0;
		else
			m_iGameHp -= iDamage;
	}

private:
	HRESULT     Add_Component();
	void        Key_Input(const _float& fTimeDelta);
	void        Mouse_Input();
	void        Mouse_Move();
	void        Mouse_Fix();
	void		Check_Dash(const _float& fTimeDelta);
	void        Set_OnTerrain();
	void        Update_State();          // 플레이어의 State 설정해주는 함수
	void        Camera_Bobbing(const _float& fTimeDelta);
	void		Apply_Gravity(const _float& fTimeDelta);		// 중력 적용 함수
	void		Check_Death();

private:
	Engine::CCalculator* m_pCalculatorCom;

public:
	// 마우스 x축 이동량을 저장
	static _long s_lMouseMoveX;

private:
	PLAYER_STATE m_eState;

	_int        m_iArmor;
	_int        m_iMax = 200;       // 체력과 아머에 사용할 최대 수치
	_int		m_iJumpCnt;			// 점프 횟수

	_float      m_fSpeed;
	_float      m_fFallSpeed;		// 추락 시 속도
	_float		m_fJumpForce;		// 점프 시 가해주는 힘
	_float		m_fDashTime;		// 대쉬 시간
	_float      m_fAngle;           // 카메라 흔들림에 적용할 각도
	_float      m_fAngleSpeed;      // 각도에 더해줄 변수
	_float      m_fDist;            // 땅에서부터 거리

	_bool       m_bIsGrounded;		// 땅에 있나
	_bool		m_bIsJumping;		// 점프 중 인가
	_bool		m_bIsDashing;		// 대쉬 중 인가
	_bool       m_bFix;
	_bool       m_bCheck;
	_bool       m_bTrigger;			// 문 열기, 스위치 누르기 등 체크용 변수
	_bool       m_bIsMoving;

	_vec3       m_vMoveDist;		// 이동거리
	_vec3		m_vVelocity;		// 가속도

public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

	// Scale = 1/tanf(fFov * 0.5f)
	// 1/(tanf(fFov * 0.5f)) = fNear
	// d:-Zv = Yn : Yv
	// 
	// //투영평면이 정사각형으로 표현되는 경우
	// Xn = d * Xv / -Zv
	// Yn = d * Yv / -Zv 
	//
	// //최종 투영 변환 공식
	// Xn = d * Xv / -Zv * fAspect
	// Yn = d * Yv / -Zv * fAspect 

public:
	virtual CDynamicCamera* Clone() override { return this; }

private:
	virtual void        Free();
};

