#pragma once
#include "CBullet.h"

class CShotgunBullet : public CBullet
{
private:
	explicit CShotgunBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShotgunBullet(const CShotgunBullet& rhs);
	virtual ~CShotgunBullet();

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

public:
	static CShotgunBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CShotgunBullet* Clone() override;
	virtual CShotgunBullet* Clone(CGameObject* _Owner)override;
};