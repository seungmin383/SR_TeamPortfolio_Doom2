#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"

namespace fs = std::filesystem;

class CObjectFactory :
    public CBase
{
    DECLARE_SINGLETON(CObjectFactory)
private:
    explicit CObjectFactory();
    virtual ~CObjectFactory();

public:
    void                Set_Dev(LPDIRECT3DDEVICE9   pGraphicDev);

    void Create_Object(OBJPROTOTYPE eprototype, OBJID eObjId, _vec3 vPos);
    CGameObject* Create_Terrain_Default(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxItv, _tchar* texTag, _vec3 pos = _vec3(0.f, 0.f, 0.f), OBJID eObjId = OBJID::OBJ_TERRAIN);
    CGameObject* Create_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxXItv, const _float& dwVtxZItv, const _float& dwVtxYItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos = _vec3(0.f, 0.f, 0.f), OBJID eObjId = OBJID::OBJ_TERRAIN);
    CGameObject* Mod_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxXItv, const _float& dwVtxZItv, const _float& dwVtxYItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos = _vec3(0.f, 0.f, 0.f), OBJID eObjId = OBJID::OBJ_TERRAIN);
    CGameObject* Load_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxXItv, const _float& dwVtxZItv, const _float& dwVtxYItv, vector<_vec3> vertices, fs::path texPath, _vec3 pos = _vec3(0.f, 0.f, 0.f), OBJID eObjId = OBJID::OBJ_TERRAIN);

private:
    LPDIRECT3DDEVICE9   m_pGraphicDev;

private:
    virtual void        Free();
};

