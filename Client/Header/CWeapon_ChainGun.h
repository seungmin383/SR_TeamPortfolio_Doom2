#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"
#include "CWeaponManager.h"

class CWeaponManager;

class CWeapon_ChainGun :    public CGameObject
{
private:
	explicit CWeapon_ChainGun(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWeapon_ChainGun(const CGameObject& rhs);
	virtual ~CWeapon_ChainGun();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	bool m_bPicked = false;

private:
	HRESULT Add_Component();
	void        Set_OnTerrain();
	void		Bill_Board();

private:
	CWeaponManager* m_pWeaponManager = nullptr;

private:
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CWeapon_ChainGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CWeapon_ChainGun* Clone() override;

private:
	virtual void Free();
};

