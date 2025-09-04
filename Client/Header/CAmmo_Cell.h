#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CWeaponManager;

class CAmmo_Cell :    public CGameObject
{
private:
	explicit CAmmo_Cell(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAmmo_Cell(const CGameObject& rhs);
	virtual ~CAmmo_Cell();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	_int    m_iCell;
	_int	m_iFrame_Cell;
	bool m_bPicked = false;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

	_float m_fDeathTimer = 0;
	virtual _int Calc_Death_Time(const _float fTimeDelta);

private:
	CWeaponManager* m_pWeaponManager = nullptr;

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CAmmo_Cell* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CAmmo_Cell* Clone() override;

private:
	virtual void Free();

};

