#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer :
    public CBase
{
private:
    explicit CLayer();
    virtual ~CLayer();

public:
    CComponent* Get_Component(COMPONENTID eID, OBJID eObjId, const _tchar* pComponentTag);
    const vector<CGameObject*>& Get_ObjectGroup(OBJID eObjId) { return m_vecObject[(UINT)eObjId]; };
    const vector<CGameObject*>* Get_AllObject() { return m_vecObject; }

    HRESULT     Add_GameObject(OBJID eObjId, CGameObject* pGameObject);

public:
    HRESULT     Ready_Layer();
    _int        Update_Layer(const _float& fTimeDelta);
    void        LateUpdate_Layer(const _float& fTimeDelta);

private:
    vector<CGameObject*>                m_vecObject[(_uint)OBJID::OBJ_END];
    vector<CGameObject*>                m_vecObjectPool[(_uint)OBJID::OBJ_END];
public:
    static CLayer* Create();

private:
    void            DeleteObjectGroup(OBJID _eTarget);
    void            DeleteAll();

    virtual void    Free();
};

END