#pragma once
#include "CBullet.h"

class CRocketBullet : public CBullet
{
private:
	explicit CRocketBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRocketBullet(const CRocketBullet& rhs);
	virtual ~CRocketBullet();

public:
	virtual     HRESULT         Ready_GameObject() override;
	virtual     _int            Update_GameObject(const _float& fTimeDelta) override;
	virtual     void            LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual     void            Render_GameObject(const _float& fTimeDelta) override;

public:
	void            OnCollision(CCollider* _pOther) override;
	void            OnCollisionEnter(CCollider* _pOther) override;
	void            OnCollisionExit(CCollider* _pOther) override;

private:
	HRESULT		Add_Component() override;
	void	    Ready_Animation();

private:
	_float  Calc_AngleToPlayer(_vec3 vLook, _vec3 vPos);
	_int	Calc_AnimState_byPlayer(_vec3 vLook, _vec3 vPos);

public:
	static CRocketBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CRocketBullet* Clone() override;
	virtual CRocketBullet* Clone(CGameObject* _Owner)override;

private:
	BULLET_STATE m_eState;
	_int m_iAnimState;
};