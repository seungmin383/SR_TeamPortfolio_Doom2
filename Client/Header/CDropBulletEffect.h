#pragma once
#include "CEffect.h"

class CDropBulletEffect : public CEffect
{
private:
	explicit CDropBulletEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDropBulletEffect(const CDropBulletEffect& rhs);
	virtual ~CDropBulletEffect();

public:
	virtual HRESULT         Ready_GameObject()override;
	virtual _int            Update_GameObject(const _float& fTimeDelta)override;
	virtual void            LateUpdate_GameObject(const _float& fTimeDelta)override;
	virtual void            Render_GameObject(const _float& fTimeDelta)override;

	virtual void Reset_Effect(_vec3 vPos) override;

	virtual void Tanpii_Movement(const _float& fTimeDelta);

public:
	CEffect* Clone() override;
	CEffect* Clone(_vec3 vPos) override;


	virtual HRESULT     Add_Component()override;

	static CDropBulletEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	_float m_fSpeed = 0.f;
	_float m_fGravity = -9.8f;
	_float m_fDrag = 0.98f;	

	_float m_fUpForce = 0.f;
	_float m_fLeftForce = 0.f;
	_vec3 m_vPos = {};
	_vec3 m_vDir = {};

private:
	void Free() override;
};

