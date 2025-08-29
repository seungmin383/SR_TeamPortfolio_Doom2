#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "CDynamicCamera.h"

class CSuits : public CGameObject
{
private:
	explicit CSuits(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSuits(const CGameObject& rhs);
	virtual ~CSuits();

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
	Engine::CRectSpriteTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CSuits* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CSuits* Clone() override;

private:
	virtual void Free();
};

