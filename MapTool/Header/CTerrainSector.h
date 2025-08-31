#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"

class CTerrainSector :
    public CGameObject
{
private:
    explicit CTerrainSector(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTerrainSector(const CGameObject& rhs);
    virtual ~CTerrainSector();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);

    void                        Set_Index(_int idx) { m_iIdx = idx; }
    _int                        Get_Index() { return m_iIdx; }

    Engine::CTerrainTexSector* Get_BufferCom() { return m_pBufferCom; }
    void        Set_BufferCom(Engine::CTerrainTexSector* pBuffer);
    void        Mod_BufferCom(const _ulong& dwCntX,
        const _ulong& dwCntZ,
        const _float& dwVtxXItv,
        const _float& dwVtxZItv,
        const _float& dwVtxYItv,
        vector<_vec3> vertices);

    void        Set_TextureCom(Engine::CTexture* pTexture);
    CTexture* Get_TextureCom() { return m_pTextureCom; }

private:
    HRESULT     Add_Component();

    HRESULT     Ready_Material();

private:
    Engine::CTerrainTexSector* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CMaterial* m_pMaterialCom;

    _int m_iIdx;
    _bool m_bBlend;

public:
    static CTerrainSector* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CTerrainSector* Clone() override;

private:
    virtual void        Free();
};

