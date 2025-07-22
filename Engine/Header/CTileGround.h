#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTileGround :
    public CVIBuffer
{
private:
    explicit CTileGround();
    explicit CTileGround(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTileGround(const CTileGround& rhs);
    virtual ~CTileGround();

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();

public:
    virtual CComponent* Clone() override;
    static CTileGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    virtual void    Free();
};

END