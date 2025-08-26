#pragma once
#include "CBullet.h"

class CBfgBullet : public CBullet
{
private:
	explicit CBfgBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBfgBullet(const CBfgBullet& rhs);
	virtual ~CBfgBullet();

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

public:
	static CBfgBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CBfgBullet* Clone() override;
	virtual CBfgBullet* Clone(CGameObject* _Owner)override;

private:
	BULLET_STATE m_eState;
	_int m_iAnimState;
};