#pragma once
#include "CBase.h"
#include"Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CSpawner :public CBase
{
    DECLARE_SINGLETON(CSpawner)

private:
    explicit CSpawner();
    virtual ~CSpawner();

private:
    virtual void        Free();
};

END
