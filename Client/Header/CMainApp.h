#pragma once
#include "CBase.h"
#include "CGraphicDev.h"
#include "CManagement.h"

class CMainApp :
    public CBase
{
private:
    explicit CMainApp();
    virtual ~CMainApp();

public:
    HRESULT         Ready_MainApp();
    int             Update_MainApp(const float& fTimeDelta);
    void            LateUpdate_MainApp(const float& fTimeDelta);
    void            Render_MainApp(const float& fTimeDelta);

private:
    HRESULT         Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
    HRESULT         Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

    _float		m_accum = 0.f;
    _float		m_frameCount = 0.f;


private:
    Engine::CGraphicDev* m_pDeviceClass;
    Engine::CManagement* m_pManagementClass;

    LPDIRECT3DDEVICE9		m_pGraphicDev;

public:
    static CMainApp* Create();

private:
    virtual void    Free();
};

