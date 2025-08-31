#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CTmpWall : public CGameObject
{
	private:
	explicit CTmpWall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTmpWall(const CTmpWall& rhs);
	virtual ~CTmpWall();

public:
	virtual     HRESULT         Ready_GameObject()override;
	virtual     _int                  Update_GameObject(const _float& fTimeDelta)override;
	virtual     void                 LateUpdate_GameObject(const _float& fTimeDelta)override;
	virtual     void                 Render_GameObject(const _float& fTimeDelta)override;

private:
	HRESULT     Add_Component();

public:
	static CTmpWall* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CTmpWall* Clone() override;
private:
	virtual void        Free();


};

