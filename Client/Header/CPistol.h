#pragma once
#include "CWeapon.h"

class CPistol :  public CWeapon
{
private:
	explicit CPistol();
	virtual ~CPistol();

public:
	virtual HRESULT Ready_GameObject() override;

public:
	virtual void Fire() override;

public:
	static CPistol* Create();

private:
	virtual void Free() override;
};

