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
    HRESULT     Add_GameObjectPool(OBJID eObjId, CGameObject* pGameObject, _int iCount);
    
    //TODO: 스테이지에 등장시킬 오브젝트의 초기 값 전달 방법 구상
    HRESULT     SetOn_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, _int iCnt = 1);
    HRESULT     SetOn_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, CGameObject* _Owner, CGameObject*& _Bullet);
    //TODO: m_vecObject를 순회하면서 회수할 오브젝트를 식별하고 회수
    HRESULT     SetOff_GameLogicObject(_int eObjId, CGameObject* pObj);
public:
    HRESULT     Ready_Layer();
    _int        Update_Layer(const _float& fTimeDelta);
    void        LateUpdate_Layer(const _float& fTimeDelta);

private:
    vector<CGameObject*>                m_vecObject[(_uint)OBJID::OBJ_END];
    vector<CGameObject*>                m_vecObjectPool[(_uint)OBJID::OBJ_END];
    map<OBJPROTOTYPE, vector<CGameObject*>>     m_mapObjectPool;
    vector<CGameObject*> m_vecPendingObject[(_uint)OBJID::OBJ_END];

public:
    static CLayer* Create();

private:
    void            DeleteObjectGroup(OBJID _eTarget);
    void            DeleteAll();
    void            DeletePool();

    virtual void    Free();
};

END