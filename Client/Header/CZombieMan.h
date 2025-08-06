#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CZombieMan :public CMonsterBase
{
    //±âº» ÃÑ ½î´Â ¸ó½ºÅÍ
private :
    explicit CZombieMan(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CZombieMan(const CMonsterBase& rhs);
    virtual ~CZombieMan();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject();

    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

#pragma region PURE
    void Movement() override;
    void Attack() override;
    void Manage_MoveState() override;
    void Change_Animation() override;
    void Thinker() override;
    void Check_Target() override;
    void Attack_Player(const _float& fTimeDelta)override;

#pragma endregion

    void Bill_Board();

private:
    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);
    void        Set_OnTerrain();

private:
    Engine::CRectSpriteTex* m_pBufferCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CCalculator* m_pCalculatorCom;
    Engine::CAnimator* m_AnimMove;
    Engine::CAnimator* m_AnimAtk;

public:
    static CZombieMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void        Free();




};

