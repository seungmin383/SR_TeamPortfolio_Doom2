#pragma once
#include "IEffect.h"

class CExplosionBlood :public IEffect
{
private:
    explicit CExplosionBlood();
	virtual ~CExplosionBlood();

public:
    void Ready_Effect() override;
    _int Update_Effect(const _float& fTimeDelta) override;

public:

    void Free() override;
};

