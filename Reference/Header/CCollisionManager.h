#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CCollider.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionManager :
    public CBase
{
private:
    explicit CCollisionManager();
    virtual ~CCollisionManager();

public:
    static void Collision_Rect_Resolve(CCollider* _Dst, CCollider* _Src, const _float& fTimeDelta);

    static void Collision_OBB_Resolve(CCollider* _Dst, CCollider* _Src);
    
    static bool Check_AABB(CCollider* _Dst, CCollider* _Src, _float* _pX, _float* _pY, _float* _pZ);
    static bool Check_OBB(CCollider* _Dst, CCollider* _Src);

    static bool Check_Mask(CCollider* _Dst, CCollider* _Src);

private:
    virtual void        Free();

};

END