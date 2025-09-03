#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CUIObject :
    public CGameObject
{
protected:
    explicit CUIObject(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CUIObject(const CUIObject& rhs);
    virtual ~CUIObject();

public:
    virtual _int Update_GameObject(const _float& fTimeDelta);

    void    Set_SizeXY(_float _fSizeX, _float _fSizeY) { m_fSizeX = _fSizeX; m_fSizeY = _fSizeY; }
    void    Set_Pos(_float _fX, _float _fY) { m_fX = _fX; m_fY = _fY; }

    void    Update_UIRect();
    _bool   Catch_Mouce();

    const _bool   Get_IsFocus() const { return m_bIsFocus; }
    void          Set_IsFocus(_bool val) { m_bIsFocus = val; }

    const _bool   Get_IsActive() const { return m_bIsActive; }
    void          Set_IsActive(_bool val) { m_bIsActive = val; }

    virtual _bool  Action() PURE;

protected:
    Engine::CRectTex*       m_pBufferCom;
    Engine::CTransform*     m_pTransformCom;
    Engine::CTexture*       m_pTextureCom;
    Engine::CCameraCom*     m_pCameraCom;

    _float				    m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

    RECT                    rcUI;
    POINT                   ptMouse;

    _bool                   m_bIsFocus;
    _bool                   m_bIsActive;

public:
    virtual CGameObject* Clone() PURE;
};

