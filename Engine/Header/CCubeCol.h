#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeCol :
    public CVIBuffer
{
private:
    explicit CCubeCol();
    explicit CCubeCol(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCubeCol(const CCubeCol& rhs);
    virtual ~CCubeCol();

public:
    virtual HRESULT         Ready_Buffer();
    virtual void            Render_Buffer();
    virtual void            Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV) override {};
    virtual void            Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) override {};

public:
    virtual CComponent* Clone() override;
    static CCubeCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

END