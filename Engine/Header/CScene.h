#pragma once
#include "CBase.h"
#include "CLayer.h"

BEGIN(Engine)

class ENGINE_DLL CScene :
    public CBase
{
protected:
    explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CScene();

public:
    CComponent* Get_Component(COMPONENTID eID,
        LAYERID eLayerId,
        OBJID eObjId,
        const _tchar* pComponentTag);

    const vector<CGameObject*> Get_ObjectGroup(LAYERID eLayerId, OBJID eObjId);

    const map<LAYERID, CLayer*> Get_Layer() { return m_mapLayer; };

    const vector<CGameObject*> Get_TotalGameObject();
    const vector<CGameObject*> Get_TotalGameObject(OBJID eObjId);

    vector<CGameObject*> Get_Terrain();
    vector<CGameObject*> Get_Wall();
    vector<CGameObject*> Get_Ceil();

    HRESULT     Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt = 1);
    HRESULT     Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, CGameObject* _Owner, CGameObject*& _Bullet);

    // (08/28 ±èÈ«ÀÎ)
    HRESULT     Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt, const std::wstring& linkId);
public:
    virtual         HRESULT     Ready_Scene();
    virtual         _int        Update_Scene(const _float& fTimeDelta);
    virtual         void        LateUpdate_Scene(const _float& fTimeDelta);
    virtual         void        Render_Scene(const _float& fTimeDelta) PURE;

    virtual         HRESULT     Load_PreSet() PURE;

    virtual         CScene*     Create() PURE;

protected:
    map<LAYERID, CLayer*>               m_mapLayer;
    LPDIRECT3DDEVICE9                   m_pGraphicDev;

protected:
    virtual void            Free();
};

END