#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CMonsterSpawner : public CGameObject
{
private:
	explicit CMonsterSpawner(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonsterSpawner(const CMonsterSpawner& rhs);
	virtual ~CMonsterSpawner();

public:
	virtual     HRESULT         Ready_GameObject()override;
	virtual     _int                  Update_GameObject(const _float& fTimeDelta)override;
	virtual     void                 LateUpdate_GameObject(const _float& fTimeDelta)override;
	virtual     void                 Render_GameObject(const _float& fTimeDelta)override;

private:
	HRESULT     Add_Component();

public:
	OBJPROTOTYPE m_ObjType;

public:
	static CMonsterSpawner* Create(LPDIRECT3DDEVICE9 pGraphicDev, OBJPROTOTYPE _Type);
	virtual CMonsterSpawner* Clone() override;
private:
	virtual void        Free();


};

