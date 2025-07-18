#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRectCol :
    public CVIBuffer
{
private:
    explicit CRectCol();
    explicit CRectCol(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CRectCol(const CRectCol& rhs);
    virtual ~CRectCol();

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();

public:
    virtual CComponent* Clone() override;
    static CRectCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    virtual void    Free();
};

END