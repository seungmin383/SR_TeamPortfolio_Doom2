#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CBullet : public CGameObject
{
protected:
	explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet();


protected:
	Engine::CRectSpriteTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CAnimator* m_pAnimCom = nullptr;

public:
	virtual void			            OnCollision(CCollider* _pOther) override;
	virtual void			            OnCollisionEnter(CCollider* _pOther) override;
	virtual void			            OnCollisionExit(CCollider* _pOther) override;

public:
	static CGameObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual CGameObject* Clone() override;

private:
    virtual void        Free()override;
};

