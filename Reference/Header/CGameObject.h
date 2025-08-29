#pragma once
#include "CBase.h"
#include "CComponent.h"
#include "CTransform.h"

BEGIN(Engine)

enum class LINKMODE : int { NONE = 0, ONE_TO_ONE = 1, ONE_TO_MANY = 2 };

class CCollider;
class ENGINE_DLL CGameObject :
    public CBase
{
protected:
    explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CGameObject(const CGameObject& rhs);
    virtual ~CGameObject();

public:
    CComponent*             Get_Component(COMPONENTID eID, const _tchar* pComponentTag);

    CCollider*              GetCollider()           { return m_pColliderCom == nullptr ? nullptr : m_pColliderCom; }
    CTransform*             Get_Transform()         { return m_pTransformCom; }

    const bool              Get_Dead() const        { return m_bDead; }
    void                    Set_Dead(_bool _bVal)   { m_bDead = _bVal; }

    _float                  Get_Speed() const        { return m_fSpeed; }
    void                    Set_Speed(_float _Speed) { m_fSpeed = _Speed; }

    _int                    Get_GameHp()const       { return m_iGameHp; };
    void                    Set_GameHp(_int _Hp)    { m_iGameHp = _Hp; }

    OBJPROTOTYPE            Get_Prototype()                   { return m_eProtoType; }
    void                    Set_Prototype(OBJPROTOTYPE eType) { m_eProtoType = eType; }

    const _uint             Get_Dmg() const     { return m_iDmg; };
    void                    Set_Dmg(_uint _D)   { m_iDmg = _D; };

    _int                    Take_Dmg(_int _Dmg) 
    {
        _int  tmp = (m_iGameHp - _Dmg); 
        m_iGameHp = tmp;     
        return  tmp; 
    };

    virtual void		    OnCollision(CCollider* _pOther) {};
    virtual void		    OnCollisionEnter(CCollider* _pOther) {};
    virtual void		    OnCollisionExit(CCollider* _pOther) {};

public:
    virtual HRESULT         Ready_GameObject();
    virtual _int            Update_GameObject(const _float& fTimeDelta);
    virtual void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual void            Render_GameObject(const _float& fTimeDelta);

    virtual CGameObject*    Clone() PURE;
    virtual CGameObject*    Clone(CGameObject* _Owner);

protected:
    map<const _tchar*, CComponent*>         m_mapComponent[ID_END];
    LPDIRECT3DDEVICE9                       m_pGraphicDev;

    CCollider*                              m_pColliderCom;     // 콜라이더(충돌체)
    CTransform*                             m_pTransformCom;
    
    OBJPROTOTYPE                            m_eProtoType;
    
    _float                                  m_fSpeed;
    _bool                                   m_bDead = FALSE;

    _uint                                   m_iDmg = 0;			//총알 데미지
    _int                                    m_iGameHp = 0;
	_int                                    m_iMaxHp = m_iGameHp;

private:
    CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);

public:
    // 링크 인터페이스: 기본 구현은 값만 보관 
    virtual void                 Set_LinkId(const std::wstring& id) { m_linkId = id; }
    virtual const std::wstring& Get_LinkId() const { return m_linkId; }

    virtual void                 Set_LinkMode(LINKMODE mode) { m_linkMode = mode; }
    virtual LINKMODE             Get_LinkMode() const { return m_linkMode; }

protected:
    std::wstring m_linkId;                
    LINKMODE     m_linkMode = LINKMODE::NONE;

protected:
    virtual void        Free();
};

END
