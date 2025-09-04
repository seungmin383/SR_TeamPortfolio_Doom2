#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"
#include "CWeaponManager.h"

class CWeaponManager;

class CWeapon_BFG : public CGameObject
{
private:
	explicit CWeapon_BFG(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWeapon_BFG(const CGameObject& rhs);
	virtual ~CWeapon_BFG();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

	WEAPON_ID	Get_WeaponID() { return m_eWeaponID; }

private:
	WEAPON_ID	m_eWeaponID = WEAPON_ID::BFG;
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
	static CWeapon_BFG* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CWeapon_BFG* Clone() override;

private:
	virtual void Free();
};
