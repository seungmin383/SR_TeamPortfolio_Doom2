#include "pch.h"
#include "CUI_StageClear.h"
#include "CDInputManager.h"
#include "CFontManager.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CStage.h"
#include "CStage2.h"
#include "CStage3.h"

CUI_StageClear* CUI_StageClear::Create(LPDIRECT3DDEVICE9 dev)
{
	CUI_StageClear* pInfo = new CUI_StageClear(dev);

	if (FAILED(pInfo->Ready_GameObject()))
	{
		Safe_Release(pInfo);
		MSG_BOX("UI_StageClear Create Failed");
		return nullptr;
	}
	return pInfo;
}

CUI_StageClear::CUI_StageClear(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CUI_StageClear::CUI_StageClear(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CUI_StageClear::~CUI_StageClear()
{
}

HRESULT CUI_StageClear::Ready_GameObject()
{
	auto* FM = Engine::CFontManager::GetInstance();
	FM->Ready_Font(m_pGraphicDev, L"ResultTitle", L"Arial", 28, 32, 900);
	FM->Ready_Font(m_pGraphicDev, L"ResultBody", L"Arial", 18, 24, 700);
	return S_OK;
}

_int CUI_StageClear::Update_GameObject(const _float& fTimeDelta)
{
    const auto& cur = CStatsManager::I().Cur();
    m_t += fTimeDelta;

    switch (m_state) {
    case State::Intro:
        if (m_t > 0.4f) { m_state = State::Counting; m_t = 0.f; }
        break;

    case State::Counting: {
        const float nps = 60.f;   // 초당 증가량
        m_k = min(cur.kills, m_k + (int)(nps * fTimeDelta));
        m_i = min(cur.items, m_i + (int)(nps * fTimeDelta));
        m_s = min(cur.secrets, m_s + (int)(nps * fTimeDelta));
        m_time = min(cur.timeSec, m_time + 120.f * fTimeDelta);

        const bool done = (m_k == cur.kills) && (m_i == cur.items) && (m_s == cur.secrets)
            && (fabsf(m_time - cur.timeSec) < 0.01f);
        if (done) { m_state = State::Wait; m_t = 0.f; }
        break;
    }

    case State::Wait:
        if (CDInputManager::GetInstance()->Get_DIKeyState(DIK_RETURN) & 0x80) {
            auto* mg = CManagement::GetInstance();
            auto* cur = mg->Get_Scene();   // 현재 씬 포인터
            SCENEID next = SCENEID::STAGE2;

            if (dynamic_cast<CStage2*>(cur))      next = SCENEID::STAGE3;
            else if (dynamic_cast<CStage3*>(cur)) next = SCENEID::LOGO; // 끝나면 로고로

            if (auto* st = dynamic_cast<CStage*>(cur)) {
                st->RequestNextScene(next);  // 즉시 전환 금지! 예약만
            }
        }
        break;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return 0;
}

void CUI_StageClear::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CUI_StageClear::Render_GameObject(const _float& dt)
{
    auto* FM = Engine::CFontManager::GetInstance();
    const auto& cur = CStatsManager::I().Cur();
    auto pct = [](int a, int b) { return b > 0 ? int((a * 100.0) / b + 0.5) : 100; };

    /// 간단 헬퍼: 좌표(x,y)를 받으면 내부에서 _vec2를 만들어 포인터로 넘김
    auto draw = [&](const wchar_t* font, const wchar_t* text, float x, float y, D3DXCOLOR color) {
        _vec2 p{ x, y };      // ★ 로컬 l-value
        FM->Render_Font(font, text, &p, color);
        };

    draw(L"ResultTitle", L"STAGE CLEAR", 30.f, 30.f, D3DXCOLOR(1, 1, 0, 1));

    wchar_t line[128];

    swprintf(line, 128, L"KILLS    %d/%d  %d%%", m_k, cur.totalKills, pct(m_k, cur.totalKills));
    draw(L"ResultBody", line, 40.f, 90.f, D3DXCOLOR(1, 1, 1, 1));

    swprintf(line, 128, L"ITEMS    %d/%d  %d%%", m_i, cur.totalItems, pct(m_i, cur.totalItems));
    draw(L"ResultBody", line, 40.f, 120.f, D3DXCOLOR(1, 1, 1, 1));

    swprintf(line, 128, L"SECRETS  %d/%d  %d%%", m_s, cur.totalSecrets, pct(m_s, cur.totalSecrets));
    draw(L"ResultBody", line, 40.f, 150.f, D3DXCOLOR(1, 1, 1, 1));

    swprintf(line, 128, L"TIME     %.1fs", m_time);
    draw(L"ResultBody", line, 40.f, 180.f, D3DXCOLOR(0.9f, 0.9f, 0.9f, 1));

    if (m_state == State::Wait) {
        draw(L"ResultBody", L"PRESS ENTER TO CONTINUE", 40.f, 230.f, D3DXCOLOR(1, 1, 1, 1));
    }
}

HRESULT CUI_StageClear::Add_Component()
{
	return S_OK;
}

void CUI_StageClear::Free()
{
	Engine::CGameObject::Free();
}
