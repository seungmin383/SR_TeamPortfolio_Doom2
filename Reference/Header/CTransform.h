#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTransform :
    public CComponent
{
private:
    explicit CTransform();
    explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTransform(const CTransform& rhs);
    virtual ~CTransform();

public:
    void        Add_Info(INFO eType, const _vec3 _info)
    {
        m_vInfo[eType] += _info;
    }
    void        Set_Info(INFO eType, const _vec3 _info)
    {
        m_vInfo[eType] = _info;
    }
    void        Get_Info(INFO eType, _vec3* pInfo)
    {
        memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
    }

    void        Move_Pos(const _vec3* pDir, const _float fSpeed, const _float& fTimeDelta)
    {
        m_vInfo[INFO_POS] += *pDir * fSpeed * fTimeDelta;
    }

    void        Rotation(ROTATION eType, const _float& fAngle)
    {
        *(((float*)&m_vAngle) + eType) += fAngle;
    }

    const   _matrix* Get_World() const { return &m_matWorld; }
    void    Get_World(_matrix* pWorld) const
    {
        *pWorld = m_matWorld;
    }

    void        Get_Pos(_vec3* pPos) const
    {
        *pPos = m_vInfo[INFO_POS];
    }
    void        Set_Pos(_float _fX, _float _fY, _float _fZ)
    {
        m_vInfo[INFO_POS] = { _fX, _fY, _fZ };
    }

    void    Set_Angle(const _vec3 _angle)
    {
        m_vAngle = _angle;
    }
    void    Get_Angle(ROTATION eType, const _vec3* pAngle)
    {
        *(((float*)&pAngle) + eType) = *(((float*)&m_vAngle) + eType);
    }
    void    Get_Angle(_vec3* pAngle) const
    {
        *pAngle = m_vAngle;
    }

    void    Set_Dir(const _vec3 _dir)
    {
        m_vDir = _dir;
    }

    void    Set_Scale(_vec3 _vScale)
    {
        m_vScale = _vScale;
    }
    const _vec3* Get_Scale() const { return &m_vScale; }
    void    Set_AxisAngle(const _float& fTimeDelta);
    void    Set_TargetPos(_vec3 _vTargetPos)
    {
        m_vTargetPos.x = _vTargetPos.x;
        m_vTargetPos.y = _vTargetPos.y;
        m_vTargetPos.z = _vTargetPos.z;
        m_bOnTarget = true;
    }

public:
    HRESULT     Ready_Transform();
    virtual _int Update_Component(const _float& fTimeDelta);
    virtual void LateUpdate_Component();

public:
    virtual CComponent* Clone();
    static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void    Free();

private:
    _vec3       m_vInfo[INFO_END];
    _vec3       m_vScale;
    _vec3       m_vAngle;

    _vec3       m_vDir;
    _vec3       m_vTargetPos;
    bool        m_bOnTarget;
    _matrix     m_matRotAxis;

    _matrix     m_matWorld;
};

END