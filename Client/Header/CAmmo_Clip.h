#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CAmmo_Clip :    public CGameObject
{
private:
	explicit CAmmo_Clip(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAmmo_Clip(const CGameObject& rhs);
	virtual ~CAmmo_Clip();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	_int    m_iClip;
	_int	m_iFrame_Clip;
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
	static CAmmo_Clip* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CAmmo_Clip* Clone() override;

private:
	virtual void Free();
};

