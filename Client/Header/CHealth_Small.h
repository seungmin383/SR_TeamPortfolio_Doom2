#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CHealth_Small :    public CGameObject
{
private:
	explicit CHealth_Small(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHealth_Small(const CGameObject& rhs);
	virtual ~CHealth_Small();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject();

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	_int    m_iHeal;
	_int	m_iFrame;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CHealth_Small* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};

