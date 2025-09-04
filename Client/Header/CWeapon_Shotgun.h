#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"
#include "CWeaponManager.h"
#include "CGameUI_Face.h"

class CWeaponManager;

class CWeapon_Shotgun :    public CGameObject
{
private:
	explicit CWeapon_Shotgun(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWeapon_Shotgun(const CGameObject& rhs);
	virtual ~CWeapon_Shotgun();

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
	static CWeapon_Shotgun* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CWeapon_Shotgun* Clone() override;

private:
	virtual void Free();
};

