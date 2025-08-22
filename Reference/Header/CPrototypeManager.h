#pragma once
#include "CBase.h"
#include "CGameObject.h"

#include "CComponent.h"

#include "CTriCol.h"
#include "CRectCol.h"
#include "CRectTex.h"
#include "CTerrainTex.h"
#include "CCubeTex.h"
#include "CRectSpriteTex.h"
#include "CTerrainTexSector.h"

#include "CTransform.h"
#include "CTexture.h"
#include "CCamera.h"
#include "CCalculator.h"
#include "CMaterial.h"
#include "CCollider.h"
#include "CAnimation.h"
#include "CAnimator.h"

BEGIN(Engine)

class ENGINE_DLL CPrototypeManager :
    public CBase
{
    DECLARE_SINGLETON(CPrototypeManager)

private:
    explicit CPrototypeManager();
    virtual ~CPrototypeManager();

public:
    //Component
    HRESULT         Ready_ComPrototype(wstring strComponentTag, CComponent* pComponent);
    CComponent*     Clone_ComPrototype(wstring strComponentTag);

    //Object
    HRESULT         Ready_ObjPrototype(OBJPROTOTYPE eObjId, CGameObject* pComponent);
    CGameObject*     Clone_ObjPrototype(OBJPROTOTYPE eObjId);
    CGameObject*     Clone_ObjPrototype(OBJPROTOTYPE eObjId, CGameObject* _Owner);
  
private:
    //Component
    CComponent*     Find_ComPrototype(wstring pComponentTag);

    //Object
    CGameObject*     Find_ObjPrototype(OBJPROTOTYPE eObjId);


private:
    map<wstring, CComponent*> m_mapComPrototype;
    map<OBJPROTOTYPE, CGameObject*>        m_mapObjPrototype;

private:
    virtual void        Free();
};

END