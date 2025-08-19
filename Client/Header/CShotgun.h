#pragma once
#include "CWeapon.h"
class CShotgun :  public CWeapon
{
private:
	explicit CShotgun();
	virtual ~CShotgun();

public:
	virtual HRESULT Ready_GameObject() override;

public:
	virtual void Fire() override;

public:
	static CShotgun* Create();

private:
	virtual void Free() override;

};