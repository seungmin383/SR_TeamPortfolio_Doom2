#pragma once
#include "IMovementStrategy.h"

class CLinearMovement :public IMovementStrategy
{
public:
	void Ready_Movement(CBullet* _Owner) override;
	void Update_Movement(CBullet* _Owner,  const _float fTimeDelta) override;
	void Free() override;
};

