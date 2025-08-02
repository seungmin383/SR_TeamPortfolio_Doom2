#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CMaterial :
    public CComponent
{
private:
    explicit CMaterial(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CMaterial(const CMaterial& rhs);
    virtual ~CMaterial();

public:
    HRESULT                 Ready_Material(const D3DMATERIAL9* pMtrlInfo);
    HRESULT                 Set_Material();

public:
    virtual CComponent*     Clone();
    static CMaterial*       Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    D3DMATERIAL9            m_tMtrlInfo;

private:
    virtual void        Free();
};

END