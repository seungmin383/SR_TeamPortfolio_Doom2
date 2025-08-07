#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CGameUI_Face : public CGameObject
{
private:
	explicit CGameUI_Face(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameUI_Face(const CGameObject& rhs);
	virtual ~CGameUI_Face();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject(const _float& fTimeDelta);

	virtual     void			OnCollision(CCollider* _pOther) override;
	virtual     void			OnCollisionEnter(CCollider* _pOther) override;
	virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
	HRESULT Add_Component();

private:
	_float m_lFrame;

private:
	Engine::CRectTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CGameUI_Face* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};