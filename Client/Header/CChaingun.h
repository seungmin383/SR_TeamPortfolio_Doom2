#pragma once
#include "CWeapon.h"
class CChaingun : public CWeapon
{
private:
	explicit CChaingun();
	virtual ~CChaingun();

public:
	virtual HRESULT Ready_GameObject() override;

public:
	virtual _bool Fire() override;

public:
	static CChaingun* Create();

private:
	virtual void Free() override;

};