#pragma once
#include "CGameObject.h"

#include "CPrototypeManager.h"

class CPlayer :
    public CGameObject
{
private:
    explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CPlayer(const CGameObject& rhs);
    virtual ~CPlayer();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

private:
    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);
    void        Set_OnTerrain();

public:
    _tchar* Get_Hp() { return (_tchar*)m_Hp; }
    int Set_Hp(int a) { return m_Hp = a; }

    _tchar* Get_Armor() { return (_tchar*)m_Armor; }
    int Set_Armor(int a) { return m_Armor = a; }

    _tchar* Get_Clip() { return (_tchar*)m_Clip; }
    int Set_Clip(int a) { return m_Clip = a; }

private:
    _int       m_Hp;
    _int       m_Armor;
    _int       m_Clip;

private:
    Engine::CRectTex*       m_pBufferCom;
    Engine::CTexture*       m_pTextureCom;
    Engine::CCalculator*    m_pCalculatorCom;

public:
    static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();
};

