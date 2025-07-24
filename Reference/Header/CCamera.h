#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :
    public CComponent
{
private:
    explicit CCamera();
    explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCamera(const CCamera& rhs);
    virtual ~CCamera();

public:
    HRESULT         Ready_Camera();
    virtual _int    Update_Component(const _float& fTimeDelta);
    virtual void    LateUpdate_Component();

    void            Set_Distance(const _float _fDistance);
    void            Chase_Target(const _vec3* pTargetPos);
    void            Look_AroundX(const _float _fVal);
    void            Look_AroundY(const _float _fVal);
    void            Rotaion(ROTATION eType, const _float _angle);

public:
    virtual CComponent* Clone();
    static CCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();

private:
    _matrix             m_matView;
    _matrix             m_matProj;

    _vec3               m_vEye;
    _vec3               m_vAt;
    _vec3               m_vUp;

    _vec3               m_vInfo[INFO_END];
    //_vec3               m_vAngle;

    //_float              m_fDistance;
};

END