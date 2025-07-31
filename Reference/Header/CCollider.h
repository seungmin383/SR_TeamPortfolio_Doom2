#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider
	: public CComponent
{
	friend class CGameObject;
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollider();

public:
	HRESULT			Ready_CCollider();
	void			Late_Update();
	void			Render();

public:
	void			OnCollision(CCollider* _pOther);
	void			OnCollisionEnter(CCollider* _pOther);
	void			OnCollisionExit(CCollider* _pOther);

public:
	void				Set_Active(bool _bVal) { m_bActive = _bVal; }
	bool				Get_Active() const { return m_bActive; }

	void				Set_Owner(CGameObject* _pObj) { m_pOwner = _pObj; }
	const CGameObject*	Get_Owner() const { return m_pOwner; }

	void				Set_ColLayer(COLLAYERID _eColLayerId) { m_eColLayerId = _eColLayerId; }
	const COLLAYERID	Get_ColLayer() const { return m_eColLayerId; }

	void				Set_ColMask(int _iColMask) { m_iColMask = _iColMask; }
	const _int			Get_ColMask() const { return m_iColMask; }

	UINT				Get_ID() { return m_iID; }

private:
	static UINT		g_iNextID;
	UINT			m_iID; // 충돌체 고유한 ID 값
	UINT			m_iCol;

	_vec3			m_vScale; //충돌체 크기
	_vec3			m_vFinalPos;
	_vec3			m_vOffsetPos; //오브젝트로부터 상대적인 위치

	COLLAYERID		m_eColLayerId;
	_int			m_iColMask;

private:
	CGameObject*	m_pOwner;
	bool			m_bActive;
};

END