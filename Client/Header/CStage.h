#pragma once
#include "CScene.h"
#include <unordered_map>
#include <limits>

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

    virtual         HRESULT     Load_PreSet() override;

private:
    HRESULT         Ready_Environment_Layer(LAYERID _eLayerId);
    HRESULT         Ready_GameLogic_Layer(LAYERID _eLayerId);
    HRESULT         Ready_UI_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Camera_Layer(LAYERID _eLayerId);
    HRESULT         Ready_Light();

public:
    static CStage* Standby(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CStage* Create() override;

    void EnterResultMode();

private:
    void ConnectDoorSwitchByLinkId();

    // Transform을 STATIC/DYNAMIC 양쪽에서 찾아주는 안전 헬퍼
    static CTransform* GetTransformPtr(Engine::CGameObject* obj)
    {
        if (!obj) return nullptr;
        auto* tr = dynamic_cast<Engine::CTransform*>(
            obj->Get_Component(Engine::ID_DYNAMIC, L"Com_Transform"));
        if (!tr) {
            tr = dynamic_cast<Engine::CTransform*>(
                obj->Get_Component(Engine::ID_STATIC, L"Com_Transform"));
        }
        return tr;
    }

    bool m_isResultMode = false;

protected:
    virtual void        Free();
};