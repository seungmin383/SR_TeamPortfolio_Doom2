#pragma once
#include "CBase.h"
#include "Engine_Define.h"

struct StageStats {
    int kills = 0, totalKills = 0;
    int items = 0, totalItems = 0;
    int secrets = 0, totalSecrets = 0;
    float timeSec = 0.f; // 누적 시간
};

class CStatsManager : public CBase
{
public:
    static CStatsManager& I() { 
        static CStatsManager s; 
        return s; 
    }

    void ResetTotals() { m_s.totalKills = m_s.totalItems = m_s.totalSecrets = 0; }
    void ResetRuntime() { m_s.kills = m_s.items = m_s.secrets = 0; m_s.timeSec = 0.f; }
    void Tick(float dt) { m_s.timeSec += dt; }

    void Totals_AddEnemy(int n = 1) { m_s.totalKills += n; }
    void Totals_AddItem(int n = 1) { m_s.totalItems += n; }
    void Totals_AddSecret(int n = 1) { m_s.totalSecrets += n; }

    // ★ 추가: 스테이지 시작 시 초기화
    void Reset() { m_s = StageStats{}; }

    // ★ 추가: 토탈 값 세팅
    void SetTotalKills(int n) { m_s.totalKills = n; }
    void SetTotalItems(int n) { m_s.totalItems = n; }
    void SetTotalSecrets(int n) { m_s.totalSecrets = n; }

    void OnEnemyKilled() { ++m_s.kills; }
    void OnItemPicked() { ++m_s.items; }
    void OnSecretFound() { ++m_s.secrets; }

    const StageStats& Cur() const { return m_s; }

private:
    StageStats m_s{};

private:
    virtual void        Free();
};

