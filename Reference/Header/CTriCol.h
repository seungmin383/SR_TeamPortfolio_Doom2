#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTriCol :
    public CVIBuffer
{
private:
    explicit CTriCol();
    explicit CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTriCol(const CTriCol& rhs);
    virtual ~CTriCol();

public:
    virtual HRESULT Ready_Buffer();
    virtual void    Render_Buffer();
    virtual void    Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV) override {};
    virtual void    Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) override {};

public:
    virtual CComponent* Clone() override;
    static CTriCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
    virtual void    Free();
};

END