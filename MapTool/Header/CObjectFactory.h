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

    CGameObject* Create_Object(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _tchar* texTag);
    CGameObject* Create_Terrain(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos = _vec3(0.f, 0.f, 0.f));
    CGameObject* Mod_Terrain(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos = _vec3(0.f, 0.f, 0.f));
    CGameObject* Load_Terrain(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, fs::path texPath, _vec3 pos = _vec3(0.f, 0.f, 0.f));

private:
    LPDIRECT3DDEVICE9   m_pGraphicDev;

private:
    virtual void        Free();
};

