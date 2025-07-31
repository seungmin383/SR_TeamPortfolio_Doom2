#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCameraCom :
    public CComponent
{
private:
    explicit CCameraCom(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CCameraCom();

public:
    HRESULT                 Ready_Camera();

private:
    LPDIRECT3DDEVICE9       m_pGraphicDev;

public:
    virtual _int            Update_Component(const _float& fTimeDelta) override;
    virtual void            LateUpdate_Component() override;

    static CCameraCom*      Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CComponent*     Clone();

private:
    void                    MakeTransformMatrix(const _vec3* pRight,
                                                const _vec3* pUp,
                                                const _vec3* pLook,
                                                const _vec3* pPos);

private:
    _matrix                 m_matView, m_matProj;
    _vec3                   m_vEye, m_vAt, m_vUp;
    _float                  m_fFov;         //시야각의 수직 영역(라디안)
    _float                  m_fAspect;      //종횡비 = 너비 / 높이
    _float                  m_fNear;        //가까운 평면까지의 거리
    _float                  m_fFar;         //먼 평면까지의 거리

private:
    virtual void            Free();
};

END