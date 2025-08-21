#pragma once
#include "CGameObject.h"
#include "CPrototypeManager.h"
#include "IMovementStrategy.h"
#include "IImpactStrategy.h"


class CBullet :public CGameObject
{
protected:
    explicit CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBullet(const CBullet& rhs);
    virtual ~CBullet();

public:
    virtual     void			OnCollision(CCollider* _pOther) override;
    virtual     void			OnCollisionEnter(CCollider* _pOther) override;
    virtual     void			OnCollisionExit(CCollider* _pOther) override;

    virtual  HRESULT Ready_GameObject();
    virtual  _int   Update_GameObject(const _float& fTimeDelta)override;

private:
    virtual HRESULT     Add_Component();

public:
    // MovementStrategy
    IMovementStrategy* GetMovementStrategy() const { return Movement_Strategy; }
    void SetMovementStrategy(IMovementStrategy* pStrategy) { Movement_Strategy = pStrategy; }

    // ImpactStrategy
    IImpactStrategy* GetImpactStrategy() const { return Impact_Strategy; }
    void SetImpactStrategy(IImpactStrategy* pStrategy) { Impact_Strategy = pStrategy; }

protected:
    Engine::CCalculator*                m_pCalculatorCom;
    Engine::CRectSpriteTex*         m_pBufferCom;
    Engine::CTexture*                   m_pTextureCom;
    Engine::CAnimator*                  m_pAnimCom;

    IMovementStrategy*     Movement_Strategy = nullptr;
    IImpactStrategy*            Impact_Strategy = nullptr;

public:
    const _vec3& Get_Pos() const { return m_vPos; }
    void         Set_Pos(const _vec3& vPos) { m_vPos = vPos; }

    _int Get_Damage() const { return m_iDamage; }
    void Set_Damage(_int iDmg) { m_iDamage = iDmg; }

    const _vec3& Get_Dir() const { return m_vDir; }
    void         Set_Dir(const _vec3& vDir) { m_vDir = vDir; }

    MONSTER_BULLET_TYPE Get_BulletType() const { return m_BulletType; }
    void                Set_BulletType(MONSTER_BULLET_TYPE eType) { m_BulletType = eType; }

    virtual HRESULT Set_TextureCom(const wchar_t* _TextureCom)
    {
        CComponent* pComponent = NULL;

        //Texture
        pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
            (CPrototypeManager::GetInstance()->Clone_ComPrototype(_TextureCom));
        if (nullptr == pComponent)
            return E_FAIL;

        m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    }

protected:
    //m_fSpeed;     //게임 오브젝트에 있음
    //m_bDead;      //게임 오브젝트에 있음
    _vec3 m_vPos;                                                       //총알의 위치
    _int        m_iDamage;                                          //총알의 데미지
    _vec3       m_vDir;                                                 //총알이 날아갈 방향 //꼭 정규화
    MONSTER_BULLET_TYPE m_BulletType;               //몰라

public:
    static CBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

    // CGameObject을(를) 통해 상속됨
    CGameObject* Clone() override;
protected:
    virtual void Free();
};

