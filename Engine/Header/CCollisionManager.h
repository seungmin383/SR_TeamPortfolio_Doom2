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

    /// <summary>
    /// 밀어내기 처리
    /// </summary>
    /// <param name="_Dst">미는 놈</param>
    /// <param name="_Src">밀리는 대상</param>
    /// <param name="fTimeDelta"></param>
    static void Collision_Rect_Resolve(CCollider* _Dst, CCollider* _Src, const _float& fTimeDelta);

    static void Collision_OBB_Resolve(CCollider* _Dst, CCollider* _Src);
    
    static bool Check_AABB(CCollider* _Dst, CCollider* _Src, _float* _pX, _float* _pY, _float* _pZ);
    static bool Check_AABB(CCollider* _Dst, CCollider* _Src);
    static bool Check_OBB(CCollider* _Dst, CCollider* _Src);

    static bool Check_Mask(CCollider* _Dst, CCollider* _Src);

    /// <summary>
    /// RayCast
    /// </summary>
    /// <param name="vRayPos">Start position</param>
    /// <param name="vRayDir">Ray direction</param>
    /// <param name="vHitPos">Ray & Collider's hit position</param>
    /// <param name="fDist">Moving distance of current frame</param>
    /// <param name="pOut">Return Collider's owner GameObject</param>
    static CGameObject* Raycast(_vec3 vRayPos, _vec3 vRayDir, _vec3& vHitPos, _float fMaxDist, LAYERID* pHitLayer);

private:
    virtual void        Free();

};

END