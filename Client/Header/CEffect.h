#pragma once
#include "CGameObject.h"
#include"CPrototypeManager.h"

class CEffect : public CGameObject
{
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect(const CGameObject& rhs);
	virtual ~CEffect();

public:
	virtual HRESULT         Ready_GameObject() ;
	virtual _int            Update_GameObject(const _float& fTimeDelta);
	virtual void            LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void            Render_GameObject(const _float& fTimeDelta);

public:
	CGameObject* Clone() override;
	virtual HRESULT     Add_Component();

protected:
	CRectSpriteTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CAnimator* m_pAnimCom = nullptr;

private:
	void Free() override;


};

