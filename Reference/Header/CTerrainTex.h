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
    virtual HRESULT Ready_Buffer(const _ulong& dwCntX,
        const _ulong& dwCntZ,
        const _ulong& dwVtxItv,
        const _tchar* pPath);
    virtual void    Render_Buffer();

public:
    virtual CComponent* Clone() override;
    static CTerrainTex* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const _ulong& dwCntX = VTXCNTX,
        const _ulong& dwCntZ = VTXCNTZ,
        const _ulong& dwVtxItv = VTXITV,
        const _tchar* pPath = L"");

public:
    const _vec3*        Get_VtxPos() { return m_pPos; }

private:
    HANDLE                  m_hFile;
    BITMAPFILEHEADER        m_fh;
    BITMAPINFOHEADER        m_ih;

    _vec3*                  m_pPos;

public:
    virtual void    Free();
};

END