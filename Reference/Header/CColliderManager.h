#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

union COLIDER_ID
{
    struct
    {
        UINT Left_Id;
        UINT Right_Id;
    };
    ULONGLONG ID;
};

class CCollider;

class ENGINE_DLL CColliderManager :
    public CBase
{
    DECLARE_SINGLETON(CColliderManager)

private:
    explicit CColliderManager();
    virtual ~CColliderManager();

public:
    /// <summary>
    /// 충돌 체크
    /// </summary>
    void        Update();

    /// <summary>
    /// 두 그룹 간의 충돌 체크 설정
    /// </summary>
    /// <param name="_eLeft"></param>
    /// <param name="_eRight"></param>
    void        CheckGroup(OBJID _eLeft, OBJID _eRight);

    /// <summary>
    /// 충돌 체크 그룹 초기화
    /// </summary>
    void        Reset()
    {
        memset(m_arrCheck, 0, sizeof(UINT) * (UINT)OBJID::OBJ_END);
    }

private:
    /// <summary>
    /// 두 그룹 간의 충돌 처리
    /// </summary>
    /// <param name="_eLeft"></param>
    /// <param name="_eRight"></param>
    void        CollisionUpdateGroup(OBJID _eLeft, OBJID _eRight);

    /// <summary>
    /// 실제 충돌 여부를 체크
    /// </summary>
    /// <param name="_pLeftCol"></param>
    /// <param name="_pRightCol"></param>
    bool        IsCollision(CCollider* _pLeftCol, CCollider* _pRightCol);

private:
    map<ULONGLONG, bool> m_mapColInfo;
    UINT m_arrCheck[(UINT)OBJID::OBJ_END];

private:
    virtual void        Free();
};

END