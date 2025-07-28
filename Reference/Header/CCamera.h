#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera :
    public CGameObject
{
protected:
    explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CCamera();

public:
    virtual         HRESULT     Ready_GameObject();
    virtual         _int        Update_GameObject(const _float& fTimeDelta);
    virtual         void        LateUpdate_GameObject(const _float& fTimeDelta);

private:
    void            LookAtLH();
    void            PerspectiveFovLH();

protected:
    _matrix     m_matView, m_matProj;
    _vec3       m_vEye, m_vAt, m_vUp;
    _float      m_fFov; //시야각의 수직 영역(라디안)
    _float      m_fAspect;//종횡비 = 너비 / 높이
    _float      m_fNear;//가까운 평면까지의 거리
    _float      m_fFar;//먼 평면까지의 거리
protected:
    virtual     void        Free();
};

END