#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CPlayer :
    public CGameObject
{
private:
    explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayer(const CGameObject& rhs);
    virtual ~CPlayer();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

private:
    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);

private:
    Engine::CRectTex* m_pBufferCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CCamera* m_pCameraCom;

public:
    static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();

private:
    _float              m_fLookX;
    _float              m_fLookY;
};

