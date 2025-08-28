#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTexSector :
    public CVIBuffer
{
private:
    explicit CTerrainTexSector();
    explicit CTerrainTexSector(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTerrainTexSector(const CTerrainTexSector& rhs);
    virtual ~CTerrainTexSector();

public:
    virtual HRESULT Ready_Buffer(const _ulong& dwCntX,
        const _ulong& dwCntZ,
        const _float& dwVtxXItv,
        const _float& dwVtxZItv,
        const _float& dwVtxYItv,
        vector<_vec3> vertices);
    virtual void    Render_Buffer();
    virtual void    Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV, _bool isReverse) override {};
    virtual void    Set_UVCoord(const _float& _fX, const _float& _fY, const _float& _fZ) override {};

public:
    virtual CComponent* Clone() override;
    static CTerrainTexSector* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        vector<_vec3> vertices,
        const _ulong& dwCntX = VTXCNTX,
        const _ulong& dwCntZ = VTXCNTZ,
        const _float& dwVtxXItv = VTXITV,
        const _float& dwVtxYItv = VTXITV,
        const _float& dwVtxZItv = VTXITV);

    static CTerrainTexSector* Create_Default(LPDIRECT3DDEVICE9 pGraphicDev,
        const _ulong& dwCntX = VTXCNTX,
        const _ulong& dwCntZ = VTXCNTZ,
        const _float& dwVtxItv = VTXITV);

    HRESULT Ready_Buffer_Default(const _ulong& dwCntX,
        const _ulong& dwCntZ,
        const _float& dwVtxItv);

public:
    const _vec3* Get_VtxPos() { return m_pPos; }
    _ulong       Get_VtxCntX() { return m_VtxCntX; }
    _ulong       Get_VtxCntZ() { return m_VtxCntZ; }
    _ulong       Get_VtxItv() { return m_VtxXItv; }

private:
    _vec3* m_pPos;

    _ulong m_VtxCntX;
    _ulong m_VtxCntZ;
    _float m_VtxXItv;
    _float m_VtxZItv;

public:
    virtual void    Free();
};

END