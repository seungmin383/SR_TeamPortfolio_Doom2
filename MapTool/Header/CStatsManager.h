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

    void OnEnemyKilled() { ++m_s.kills; }
    void OnItemPicked() { ++m_s.items; }
    void OnSecretFound() { ++m_s.secrets; }

    const StageStats& Cur() const { return m_s; }

private:
    StageStats m_s{};

private:
    virtual void        Free();
};

