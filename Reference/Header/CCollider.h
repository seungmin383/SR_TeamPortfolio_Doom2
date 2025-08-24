#pragma once
#include "CComponent.h"
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCollider
	: public CComponent
{
	friend class CGameObject;
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();

public:
	/// <summary>
	/// Ready_GameObject에서 호출한다
	/// </summary>
	/// <param name="_pObj">오브젝트</param>
	/// <param name="_vCenter">오브젝트의 중심</param>
	/// <param name="_vScale">충돌체 크기</param>
	/// <param name="_vOffset">오브젝트와의 상대적 위치</param>
	/// <returns></returns>
	HRESULT				Ready_CCollider(CGameObject* _pObj, _vec3 _vPos, _vec3 _vScale, _vec3 _vOffset,
		COLLAYERID _eColLayerId, _int _iColMask);
	/// <summary>
	/// 충돌체 위치 업데이트
	/// 오브젝트 위치와 동기화한다
	/// </summary>
	void				Late_Update();
	void				Render(_bool _colRend);

	virtual CComponent*		Clone();
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGarphicDev);

public:
	void				OnCollision(CCollider* _pOther);
	void				OnCollisionEnter(CCollider* _pOther);
	void				OnCollisionExit(CCollider* _pOther);


	void				HitRay(CCollider* _pOther);

public:
	void				Set_Active(bool _bVal) { m_bActive = _bVal; }
	bool				Get_Active() const { return m_bActive; }

	void				Set_Owner(CGameObject* _pObj) { m_pOwner = _pObj; }
	CGameObject*		Get_Owner() const { return m_pOwner; }

	const _vec3			Get_Scale() const { return m_vScale; }
	void				Set_Scale(_vec3 vScale) { m_vScale = vScale; }

	const _vec3			Get_FinalPos() const { return m_vFinalPos; }

	bool				Is_RayHit() const { return m_bHit; }

	/// <summary>
	/// 충돌 분류
	/// ex)
	/// m_eColLayerId = COL_PLAYER;
	/// m_iColMask = COL_ITEM | COL_OBJECT | COL_TERRAIN | COL_WALL | COL_ATTACK;
	///
	/// 충돌 확인
	/// if ( (PlayerCollider->m_iColMask & ItemCollider->m_eColLayerId) != 0)
	///		//처리 코드
	/// </summary>
	/// <param name="_eColLayerId"></param>
	void				Set_ColLayer(COLLAYERID _eColLayerId) { m_eColLayerId = _eColLayerId; }
	const COLLAYERID	Get_ColLayer() const { return m_eColLayerId; }

	/// <summary>
	/// 충돌 분류
	/// ex)
	/// m_eColLayerId = COL_PLAYER;
	/// m_iColMask = COL_ITEM | COL_OBJECT | COL_TERRAIN | COL_WALL | COL_ATTACK;
	///
	/// 충돌 확인
	/// if ( (PlayerCollider->m_iColMask & ItemCollider->m_eColLayerId) != 0)
	///		//처리 코드
	/// </summary>
	/// <param name="_iColMask"></param>
	void				Set_ColMask(int _iColMask) { m_iColMask = _iColMask; }
	const _int			Get_ColMask() const { return m_iColMask; }

	UINT				Get_ID() { return m_iID; }

	/// <summary>
	/// 충돌체 정점 반환
	/// </summary>
	const _vec3*		Get_WorldCollisionBox() const { return m_ColliderBox; };
	void				Update_Box();

private:
	static UINT			g_iNextID;
	UINT				m_iID; // 충돌체 고유한 ID 값
	UINT				m_iCol;

	_vec3				m_ColliderBox[8];

	_vec3				m_vPos;
	_vec3				m_vScale; //충돌체 크기
	_vec3				m_vScaleHalf;
	_vec3				m_vOffsetPos; //오브젝트로부터 상대적인 위치
	_vec3				m_vFinalPos;

	COLLAYERID			m_eColLayerId;
	_int				m_iColMask;

	_bool				m_bHit;
	_float				m_fHitDelay;
	_float				m_fHitTime;

	LPD3DXLINE			m_pLine;		// 콜라이더 그리기용 멤버 변수

private:
	CGameObject*		m_pOwner;
	bool				m_bActive;

private:
	virtual void		Free();
};

END