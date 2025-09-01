#pragma once
#include "CEffect.h"

class CExplosionBloodEffect : public CEffect
{
private:
	explicit CExplosionBloodEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CExplosionBloodEffect(const CExplosionBloodEffect& rhs);
	virtual ~CExplosionBloodEffect();

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

	static CExplosionBloodEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	_int m_iCurFrame = 0;
private:
	void Free() override;

};

