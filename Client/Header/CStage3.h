#pragma once
#include "CScene.h"
class CStage3 :
    public CScene
{
public:
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

private:
    // 결과창 띄우기 위한 bool 변수
    bool m_isResultMode = false;

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

protected:
    virtual void        Free();
};

