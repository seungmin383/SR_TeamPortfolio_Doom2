#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTex :
    public CVIBuffer
{
private:
    explicit CTerrainTex();
    explicit CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTerrainTex(const CTerrainTex& rhs);
    virtual ~CTerrainTex();

public:
    virtual HRESULT Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
    virtual void    Render_Buffer();

public:
    virtual CComponent* Clone() override;
    static CTerrainTex* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const _ulong& dwCntX = VTXCNTX,
        const _ulong& dwCntZ = VTXCNTZ,
        const _ulong& dwVtxItv = VTXITV);

private:
    HANDLE                  m_hFile;
    BITMAPFILEHEADER        m_fh;
    BITMAPINFOHEADER        m_ih;

public:
    virtual void    Free();
};

END