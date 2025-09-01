#pragma once
#include "CEffect.h"

class CImpactBulletEffect : public CEffect
{
private:
	explicit CImpactBulletEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CImpactBulletEffect(const CImpactBulletEffect& rhs);
	virtual ~CImpactBulletEffect();

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

	static CImpactBulletEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;

};

