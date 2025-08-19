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

public :
    void Ready_Spawner();

public:
    void CreateMonster(_uint _MonsterID, _vec3 _InitPos, _vec3 _InitLook);

private:
    virtual void        Free();
};

END
