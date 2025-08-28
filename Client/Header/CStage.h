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

    // 결과창 띄우는 함수
    void EnterResultMode();

    // 다음 씬으로 넘기는 함수 
    void RequestNextScene(SCENEID id);

    bool IsTransitioning() const noexcept { return m_pendingNext != SCENEID::SCENE_END; }
    bool IsResultMode() const noexcept { return m_isResultMode; } // 이미 있다면 그대로 사용

private:
    void ConnectDoorSwitchByLinkId();

    // 설치가 끝난 뒤 Management에서 (obj*, tag) 꺼내와 Set_LinkId 적용
    void ApplyInteractiveTagsFromManagement();  

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

    // 결과창 띄우기 위한 bool 변수
    bool m_isResultMode = false;

    // 다음 씬의 아이디
    SCENEID m_pendingNext = SCENEID::SCENE_END;

protected:
    virtual void        Free();
};