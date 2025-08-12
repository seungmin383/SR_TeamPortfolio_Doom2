#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CArmor_Small :    public CGameObject
{
private:
	explicit CArmor_Small(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CArmor_Small(const CGameObject& rhs);
	virtual ~CArmor_Small();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	_int    m_iArmor;
	_int	m_iFrame_Armror;
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
	static CArmor_Small* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CArmor_Small* Clone() override;

private:
	virtual void Free();
};

