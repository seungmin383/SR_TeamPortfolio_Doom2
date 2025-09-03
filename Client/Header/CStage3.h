#pragma once
#include "CScene.h"
class CStage3 :
    public CScene
{
private:
    explicit CStage3(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CStage3();

public:
    virtual         HRESULT     Ready_Scene();
    virtual         _int        Update_Scene(const _float& fTimeDelta);
    virtual         void        LateUpdate_Scene(const _float& fTimeDelta);
    virtual         void        Render_Scene(const _float& fTimeDelta);

    virtual         HRESULT     Load_PreSet() override;

private:
    HRESULT         Ready_Environment_Layer(LAYERID _eLayerId);
    HRESULT         Ready_GameLogic_Layer(LAYERID _eLayerId);
    HRESULT         Ready_UI_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Camera_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Light();

public:
    static CStage3* Standby(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CStage3* Create() override;

public:
    // 결과창 띄우는 함수(08/31 김홍인)
    void EnterResultMode();

    // 다음 씬으로 넘기는 함수 (08/31 김홍인)
    void RequestNextScene(SCENEID id);

private:
    // 결과창 띄우기 위한 bool 변수
    bool m_isResultMode = false;

    // 다음 씬의 아이디
    SCENEID m_pendingNext = SCENEID::SCENE_END;

private:
    void ConnectDoorSwitchByLinkId();

    // Transform을 STATIC/DYNAMIC 양쪽에서 찾아주는 안전 헬퍼
    static CTransform* GetTransformPtr(Engine::CGameObject* obj)
    {
        if (!obj) return nullptr;
        auto* tr = dynamic_cast<Engine::CTransform*>(
            obj->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
        if (!tr)
        {
            tr = dynamic_cast<Engine::CTransform*>(
                obj->Get_Component(Engine::ID_STATIC, L"Com_Transform"));
        }
        return tr;
    }

    // 추가: 클리어 목표 타입
    enum class GoalType { KillBoss };
    enum class GoalSource { None, Boss };
    enum class GoalTiePolicy { PreferEarliest, PreferBoss };

    GoalTiePolicy m_tiePolicy = GoalTiePolicy::PreferEarliest;
    GoalType m_goal = GoalType::KillBoss; // 기본: 보스 처치형
    GoalSource     m_goalSource = GoalSource::None;
    bool           m_goalLatched = false; // 한번만 결과 발생하도록 잠금
    float          m_goalBossTime = FLT_MAX;
    _float         m_goalKillTime = FLT_MAX;

    // 목표/상태 체크
    void InitTotals();            // 총 몬스터 수 등 세팅
    void CheckStageGoal();        // 매 프레임 조건 검사
    bool IsBossAlive();          // 보스 생존 여부

    // ★ 추가: 최종 원인 선택 & 라치
    void ResolveAndLatchGoal();
protected:
    virtual void        Free();
};

