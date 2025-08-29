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
    void    Set_SizeXY(_float _fSizeX, _float _fSizeY) { m_fSizeX = _fSizeX; m_fSizeY = _fSizeY; }
    void    Set_Pos(_float _fX, _float _fY) { m_fX = _fX; m_fY = _fY; }

    void    Update_UI();
    _bool   Catch_Mouce();

protected:
    Engine::CRectTex*       m_pBufferCom;
    Engine::CTransform*     m_pTransformCom;
    Engine::CTexture*       m_pTextureCom;
    Engine::CCameraCom*     m_pCameraCom;

    _float				    m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

    RECT                    rcUI;
    POINT                   ptMouse;

public:
    virtual CGameObject* Clone() override;
};

