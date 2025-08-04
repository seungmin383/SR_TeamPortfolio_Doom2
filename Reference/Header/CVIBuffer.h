#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer :
    public CComponent
{
protected:
    explicit CVIBuffer();
    explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CVIBuffer(const CVIBuffer& rhs);
    virtual ~CVIBuffer();

public:
    virtual HRESULT         Ready_Buffer();
    virtual void            Render_Buffer();

    virtual void            Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV) PURE;
    virtual void            Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) PURE;

protected:
    LPDIRECT3DVERTEXBUFFER9     m_pVB;
    LPDIRECT3DINDEXBUFFER9      m_pIB;

    _ulong                      m_dwVtxSize;
    _ulong                      m_dwVtxCnt;
    _ulong                      m_dwTriCnt;
    _ulong                      m_dwFVF;

    _ulong                      m_dwIdxSize;
    D3DFORMAT                   m_IdxFmt;

public:
    virtual void    Free();
};

END