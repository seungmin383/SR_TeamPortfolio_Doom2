#pragma once
#include "CBase.h"
#include "CComponent.h"

#include "CTriCol.h"
#include "CRectCol.h"
#include "CRectTex.h"
#include "CTerrainTex.h"
#include "CCubeTex.h"

#include "CTransform.h"
#include "CTexture.h"
#include "CCamera.h"
#include "CCalculator.h"

BEGIN(Engine)

class ENGINE_DLL CPrototypeManager :
    public CBase
{
    DECLARE_SINGLETON(CPrototypeManager)

private:
    explicit CPrototypeManager();
    virtual ~CPrototypeManager();

public:
    HRESULT         Ready_Prototype(const _tchar* pComponentTag, CComponent* pComponent);
    CComponent*     Clone_Prototype(const _tchar* pComponentTag);

private:
    CComponent*     Find_Prototype(const _tchar* pComponentTag);

private:
    map<const _tchar*, CComponent*> m_mapPrototype;

private:
    virtual void        Free();
};

END