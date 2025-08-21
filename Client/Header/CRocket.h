#pragma once
#include "CWeapon.h"
class CRocket : public CWeapon
{
private:
	explicit CRocket();
	virtual ~CRocket();

public:
	virtual HRESULT Ready_GameObject() override;

public:
	virtual _bool Fire() override;

public:
	static CRocket* Create();

private:
	virtual void Free() override;

};