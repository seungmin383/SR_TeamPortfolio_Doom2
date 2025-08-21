#pragma once
#include "CBase.h"
#include"Engine_Define.h"

class CBullet;
class IImpactStrategy :public CBase
{
public:
	virtual ~IImpactStrategy() {};

	virtual void Ready_Impact(CBullet* _Owner)PURE;
	virtual void Update_Impact(CBullet* _Owner, const _float fTimeDelta)PURE;
};

