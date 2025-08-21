#pragma once
#include "CBase.h"
#include"Engine_Define.h"

class CBullet;
class IMovementStrategy :public CBase
{
public:
	virtual ~IMovementStrategy() {};

	virtual void Ready_Movement(CBullet* _Owner)PURE;
	virtual void Update_Movement( CBullet* _Owner, const _float fTimeDelta)PURE;
};

