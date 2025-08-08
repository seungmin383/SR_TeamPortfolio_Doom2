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
    virtual void    Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV, _bool isReverse) override {};
    virtual void    Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) override {};

public:
    virtual CComponent* Clone() override;
    static CRectTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

END
