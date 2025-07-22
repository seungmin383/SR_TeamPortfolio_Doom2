#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRectTex :
    public CVIBuffer
{
private:
    explicit CRectTex();
    explicit CRectTex(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CRectTex(const CRectTex& rhs);
    virtual ~CRectTex();

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();

public:
    virtual CComponent* Clone() override;
    static CRectTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

END
