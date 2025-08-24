#pragma once
#include "CBase.h"
#include "CScene.h"

BEGIN(Engine)

class CGameUI_Face;

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

    HRESULT         Set_Scene(CScene* pScene);
    CScene*         Get_Scene() { return m_pScene; }

    HRESULT         Add_Scene(SCENEID _eSceneId, CScene* _pScene);
    HRESULT         Change_Scene(SCENEID _eSceneId);

    HRESULT         Set_Player(CGameObject* pObj);
    CGameObject*    Get_Player() { return m_pPlayer; }

    SCENEID         Get_CurSceneId() { return m_eCurSceneId; }

    /// <summary>
    /// Game Logic Layer에 추가한다
    /// </summary>
    /// <param name="eObjPrototype"></param>
    /// <returns></returns>
    HRESULT         Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos = _vec3(0.f, 0.f, 0.f), _int iCnt = 1);
    HRESULT         Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId, _vec3 vPos, CGameObject* _Owner, CGameObject*& _Bullet );

public:
    _int        Update_Scene(const _float& fTimeDelta);
    void        LateUpdate_Scene(const _float& fTimeDelta);
    void        Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fTimeDelta);

    // 임시로 키 상호작용을 위한 코드 추가(08/23 김홍인)
    _bool        Is_KeyOwned() const { return m_bHasKey; }
    void         Set_KeyOwned(_bool b) { m_bHasKey = b; }

    // 무기 획득 시 얼굴 변화는 효과를 주기위해 추가(08/24 김홍인)
    void        Set_FaceUI(CGameUI_Face* p) { m_pFaceUI = p; }
    CGameUI_Face* Get_FaceUI() const { return m_pFaceUI; }

private:
    CScene*                 m_pScene;
    CScene*                 m_pArrScene[(_uint)SCENEID::SCENE_END];
    SCENEID                 m_eCurSceneId;

    CGameObject*            m_pPlayer;

    // 임시로 키 상호작용을 위한 변수 추가(08/23 김홍인)
    _bool                   m_bHasKey = false;

    // 무기 획득 시 얼굴 변화는 효과를 주기위해 추가(08/24 김홍인)
    CGameUI_Face* m_pFaceUI = nullptr;

public:
    virtual     void        Free();
};

END