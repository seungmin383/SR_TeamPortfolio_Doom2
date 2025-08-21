#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBulletMananger :public CBase
{
	DECLARE_SINGLETON(CBulletMananger);

private:
    explicit CBulletMananger();
    virtual ~CBulletMananger();

    virtual void aa();

private:
    void Free() override;

};
END

