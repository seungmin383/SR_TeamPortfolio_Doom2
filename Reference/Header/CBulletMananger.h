#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CBullet.h"

BEGIN(Engine)

class ENGINE_DLL CBulletMananger :public CBase
{
	DECLARE_SINGLETON(CBulletMananger);

private:
    explicit CBulletMananger();
    virtual ~CBulletMananger();

    virtual void aa();


public:

private:
    void Free() override;

};
END

