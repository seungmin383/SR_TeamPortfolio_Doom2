#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"

class ENGINE_DLL CObjectFactory :
    public CBase
{
    DECLARE_SINGLETON(CObjectFactory)
private:
    explicit CObjectFactory();
    virtual ~CObjectFactory();

public:
    void                Set_Dev(LPDIRECT3DDEVICE9   pGraphicDev);

    CGameObject*        Create_Object(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _tchar* texTag);
    CGameObject*        Create_Terrain(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _vec3 vScale, _tchar* texTag);

private:
    LPDIRECT3DDEVICE9   m_pGraphicDev;

private:
    virtual void        Free();
};

