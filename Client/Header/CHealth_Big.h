#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CHealth_Big :    public CGameObject
{
private:
	explicit CHealth_Big(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHealth_Big(const CGameObject& rhs);
	virtual ~CHealth_Big();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	_int    m_iHeal;
	bool m_bPicked = false;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CHealth_Big* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CHealth_Big* Clone() override;

private:
	virtual void Free();
};

