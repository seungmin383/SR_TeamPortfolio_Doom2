#pragma once
#include "CGameObject.h"
#include "CFontManager.h"
#include "CStatsManager.h"
#include "CPrototypeManager.h"

class CUI_StageClear :    public CGameObject
{
public:
    enum class State { Intro, Counting, Wait };

    static CUI_StageClear* Create(LPDIRECT3DDEVICE9 dev);
    virtual CUI_StageClear* Clone() override { return this; }

private:
    explicit CUI_StageClear(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CUI_StageClear(const CGameObject& rhs);
    virtual ~CUI_StageClear();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject(const _float& dt) override;

public:
    _float Get_m_t() { return m_t; }
    _int   Get_m_k() { return m_k; }
    _int   Get_m_i() { return m_i; }
    _float Get_Time() { return m_time; }

private:
    HRESULT Add_Component();

    void  Render_Number(_int number, _float baseX, _float baseY, _float scaleX, _float scaleY);
    void  Render_Sprite(Engine::CTexture* texture, _float x, _float y, _float scaleX, _float scaleY);
private:
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTexture* m_pTextureClearCom;
    Engine::CTexture* m_pTextureKillsCom;
    Engine::CTexture* m_pTextureItemsCom;
    Engine::CTexture* m_pTextureTimesCom;
    Engine::CTexture* m_pTextureNumber;
    Engine::CCalculator* m_pCalculatorCom;


    State  m_state = State::Intro;
    float  m_t = 0.f;
    int    m_k = 0, m_i = 0, m_s = 0;   // 표시용 카운터
    float  m_time = 0.f;                // 표시용 타이머

private:
    virtual void Free();
};

