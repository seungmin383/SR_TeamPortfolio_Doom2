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

    // 기존과 비슷한 시그니처: 프로토/ID/위치/개수 + 링크 태그(08/28 김홍인)
    HRESULT Set_GameLogicObject(OBJPROTOTYPE eObjPrototype, OBJID eObjId,
        const _vec3& vPos, _int iCnt,
        const std::wstring& linkId);

    // Client가 태그 적용할 때 꺼내 쓰는 헬퍼
    void Pop_InteractiveTags(OBJID eObjId, std::vector<CLayer::SInteractiveTag>& outList);
public:
    _int        Update_Scene(const _float& fTimeDelta);
    void        LateUpdate_Scene(const _float& fTimeDelta);
    void        Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fTimeDelta);

    // 키 상호작용을 위한 코드 추가(08/23 김홍인)
    _bool        Is_KeyOwned() const { return m_bHasKey; }
    void         Set_KeyOwned(_bool b) { m_bHasKey = b; }

    // MainMenu 종료시 오류 나는 것 방지용(08/26 김홍인)
    void  RequestQuit() { m_quit = true; }
    bool  QuitRequested() const { return m_quit; }

    // 일시 정지 기능을 위한 함수
    _bool   Is_Paused() const { return m_bPaused; }
    void    Set_Paused(_bool b) {
        m_bPaused = b;
        m_timeScale = m_bPaused ? 0.f : 1.f;
    }
    void    Toggle_Pause() { Set_Paused(!m_bPaused); }

    // Δt 스케일 (정지: 0, 평상: 1)
    _float  ScaleDelta(const _float& fTimeDelta) const { return fTimeDelta * m_timeScale; }

private:
    CScene*                 m_pScene;
    CScene*                 m_pArrScene[(_uint)SCENEID::SCENE_END];
    SCENEID                 m_eCurSceneId;

    CGameObject*            m_pPlayer;

    // 키 상호작용을 위한 변수 추가(08/23 김홍인)
    _bool                   m_bHasKey = false;
    _bool                   m_quit = false;

    // 일시정지 기능을 위한 변수(08/27 김홍인)
    _bool                   m_bPaused = false;
    _float                  m_timeScale = 1.f;
public:
    virtual     void        Free();
};

END