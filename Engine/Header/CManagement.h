#pragma once
#include "CBase.h"
#include "CScene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement :
    public CBase
{
    DECLARE_SINGLETON(CManagement);

private:
    explicit CManagement();
    virtual ~CManagement();

public:
    CComponent* Get_Component(COMPONENTID eID,
        LAYERID _eLayerId,
        OBJID _eObjId,
        const _tchar* pComponentTag);

    const vector<CGameObject*>& Get_ObjectGroup(LAYERID _eLayerId, OBJID _eObjId);

public:
    HRESULT         Set_Scene(CScene* pScene);
    CScene*         Get_Scene() { return m_pScene; }

    _int        Update_Scene(const _float& fTimeDelta);
    void        LateUpdate_Scene(const _float& fTimeDelta);
    void        Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    CScene*                 m_pScene;

public:
    virtual     void        Free();
};

END