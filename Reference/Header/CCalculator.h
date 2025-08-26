#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;
class CTerrainTexSector;
class CGameObject;

class ENGINE_DLL CCalculator :
    public CComponent
{
private:
    explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CCalculator();

public:
    HRESULT         Ready_Calculator();
    _float          Compute_HeightOnTerrain(const _vec3* pPos,
                                            const _vec3* pTerrainVtxPos,
                                            const _ulong& dwCntX,
                                            const _ulong& dwCntZ,
                                            const _ulong& dwVtxItv = VTXITV);
    _float          Compute_HeightOnTerrain(_vec3 pPos,
                                            _ulong dwIndex,
                                            const _vec3* pTerrainVtxPos,
                                            _ulong dwCntX,
                                            _ulong dwCntZ,
                                            _ulong dwVtxItv,
                                            _bool RightUp);

    _bool           RayScan_OnTerrain(CTerrainTexSector* pTerrainSectorBufferCom, CTransform* pTerrainTransformCom, _vec3* targetPos, _vec3* objPos);

    _vec3           Picking_OnTerrain(HWND hWnd,
                                    CTerrainTex* pTerrainBufferCom,
                                    CTransform* pTerrainTransformCom);

    _bool           Picking_GameObject(HWND hWnd);
    _bool           Picking_MapTool(HWND hWnd, CTerrainTexSector* pTerrainSectorBufferCom, CTransform* pTerrainTransformCom, _vec3* targetPos, _float* dist);

    _bool           Picking_TerrainSearch(HWND hWnd, CTerrainTexSector* pTerrainSectorBufferCom);
    
public:
    static CCalculator*     Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CComponent*      Clone();

private:
    virtual void            Free();
};

END