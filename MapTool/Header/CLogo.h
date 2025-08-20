#pragma once
#include "CScene.h"
#include "CLoading.h"

class CLogo :
    public CScene
{
public:
    explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CLogo();

public:
    virtual         HRESULT     Ready_Scene();
    virtual         _int        Update_Scene(const _float& fTimeDelta);
    virtual         void        LateUpdate_Scene(const _float& fTimeDelta);
    virtual         void        Render_Scene(const _float& fTimeDelta);

    virtual         HRESULT     Load_PreSet() override { return S_OK; }

private:
    HRESULT         Ready_Environment_Layer(LAYERID _eLayerId);
    HRESULT         Ready_GameLogic_Layer(LAYERID _eLayerId) { return 0; }
    HRESULT         Ready_UI_Layer(LAYERID _eLayerId) { return 0; }

private:
    HRESULT         Ready_Prototype();

private:
    CLoading* m_pLoading;

public:
    static CLogo* Standby(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CLogo* Create() override;

protected:
    virtual void            Free();
};

