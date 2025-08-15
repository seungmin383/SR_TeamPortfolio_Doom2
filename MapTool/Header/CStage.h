#pragma once
#include "CScene.h"
class CStage :
    public CScene
{
public:
    explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CStage();

public:
    virtual         HRESULT     Ready_Scene();
    virtual         _int        Update_Scene(const _float& fTimeDelta);
    virtual         void        LateUpdate_Scene(const _float& fTimeDelta);
    virtual         void        Render_Scene(const _float& fTimeDelta);

private:
    HRESULT         Ready_Environment_Layer(LAYERID _eLayerId);
    HRESULT         Ready_GameLogic_Layer(LAYERID _eLayerId);
    HRESULT         Ready_UI_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Camera_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Light();

public:
    static CStage* Standby(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CStage* Create() override;

protected:
    virtual void        Free();
};