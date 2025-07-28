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

public:
    _bool           ReadRawFile(string _strFileName);
    int             Get_HeightMapEntry(int _row, int _col);
    void            Set_HeightMapEntry(int _row, int _col, int _value);

private:
    vector<int>         m_vecHeightmap;

    int                 m_iNumVertsPerRow;
    int                 m_iNumVertsPerCol;
    int                 m_iCellSpacing;

    int                 m_iNumCellsPerRow;
    int                 m_iNumCellsPerCol;
    int                 m_iWidth;
    int                 m_iDepth;
    int                 m_iNumVertices;
    int                 m_iNumTriangles;

    float               m_fHeightScale;

public:
    virtual void    Free();
};

END