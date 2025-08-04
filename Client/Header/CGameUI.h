#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CGameUI :    public CGameObject
{
private:
	explicit CGameUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameUI(const CGameObject& rhs);
	virtual ~CGameUI();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject();

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	HRESULT Add_Component();

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CGameUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};

