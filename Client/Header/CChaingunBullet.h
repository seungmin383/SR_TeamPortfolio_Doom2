#pragma once
#include "CBullet.h"

class CChaingunBullet : public CBullet
{
private:
	explicit CChaingunBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CChaingunBullet(const CChaingunBullet& rhs);
	virtual ~CChaingunBullet();

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
	static CChaingunBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CChaingunBullet* Clone() override;
	virtual CChaingunBullet* Clone(CGameObject* _Owner)override;
};