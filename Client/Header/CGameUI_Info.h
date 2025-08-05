#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CFontManager.h"
#include "CPlayer.h"
#include "CPlayerWeapon.h"

class CGameUI_Info :    public CGameObject
{
private:
	explicit CGameUI_Info(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameUI_Info(const CGameObject& rhs);
	virtual ~CGameUI_Info();

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

public:
	void Set_Player(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

private:
	CPlayer* m_pPlayer;

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CGameUI_Info* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};

