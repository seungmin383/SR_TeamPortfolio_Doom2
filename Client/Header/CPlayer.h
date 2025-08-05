#pragma once
#include "CCharacter.h"

#include "CPrototypeManager.h"

class CPlayer :
    public CCharacter
{
private:
    explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayer(const CCharacter& rhs);
    virtual ~CPlayer();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
    void        Movement() override;
    void        Attack() override;

    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);
    void        Mouse_Move();
    void        Set_OnTerrain();

private:
    Engine::CRectTex*       m_pBufferCom;
    Engine::CTexture*       m_pTextureCom;
    Engine::CCalculator*    m_pCalculatorCom;

public:
    static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

