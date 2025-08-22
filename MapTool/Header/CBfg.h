#pragma once
#include "CWeapon.h"
class CBfg : public CWeapon
{
private:
	explicit CBfg();
	virtual ~CBfg();

public:
	virtual HRESULT Ready_GameObject() override;

public:
	virtual _bool Fire() override;

public:
	static CBfg* Create();

private:
	virtual void Free() override;

};