#pragma once
#include "CEffect.h"

class CBfgEffect : public CEffect
{
private:
	explicit CBfgEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBfgEffect(const CBfgEffect& rhs);
	virtual ~CBfgEffect();

public:
	virtual HRESULT         Ready_GameObject()override;
	virtual _int            Update_GameObject(const _float& fTimeDelta)override;
	virtual void            LateUpdate_GameObject(const _float& fTimeDelta)override;
	virtual void            Render_GameObject(const _float& fTimeDelta)override;

	virtual void Reset_Effect(_vec3 vPos) override;


public:
	CEffect* Clone() override;
	CEffect* Clone(_vec3 vPos) override;


	virtual HRESULT     Add_Component()override;

	static CBfgEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;
};

