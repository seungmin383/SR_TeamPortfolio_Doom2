#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRectSpriteTex :
    public CVIBuffer
{
private:
    explicit CRectSpriteTex();
    explicit CRectSpriteTex(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CRectSpriteTex(const CRectSpriteTex& rhs);
    virtual ~CRectSpriteTex();

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();
    virtual void    Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV) override;
    virtual void    Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) override {};

public:
    virtual CComponent* Clone() override;
    static CRectSpriteTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

END