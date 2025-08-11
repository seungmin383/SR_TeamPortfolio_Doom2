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

    const vector<CGameObject*>& Get_ObjectGroup(LAYERID eLayerId, OBJID eObjId);

    const map<LAYERID, CLayer*>& Get_Layer() { return m_mapLayer; };

    const vector<CGameObject*> Get_TotalGameObject();
    const vector<CGameObject*> Get_TotalGameObject(OBJID eObjId);

    HRESULT     Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt = 1);

public:
    virtual         HRESULT     Ready_Scene();
    virtual         _int        Update_Scene(const _float& fTimeDelta);
    virtual         void        LateUpdate_Scene(const _float& fTimeDelta);
    virtual         void        Render_Scene(const _float& fTimeDelta) PURE;

    virtual         CScene*     Create() PURE;

protected:
    map<LAYERID, CLayer*>               m_mapLayer;
    LPDIRECT3DDEVICE9                   m_pGraphicDev;

protected:
    virtual void            Free();
};

END