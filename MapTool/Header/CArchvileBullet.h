#pragma once
#include "CBullet.h"

class CArchvileBullet :  public CBullet
{
private:
    explicit CArchvileBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CArchvileBullet(const CArchvileBullet& rhs);
    virtual ~CArchvileBullet();

public:
    virtual     HRESULT         Ready_GameObject()override;
    virtual     _int            Update_GameObject(const _float& fTimeDelta)override;
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta)override;
    virtual     void            Render_GameObject(const _float& fTimeDelta)override;

#pragma endregion

    virtual void Check_Anim_Is_Finish();
    //몬스터 공격이 끝나고 
    virtual void Calc_Time_EndAttack(const _float& fTimeDelta);

public:
	_bool Get_isFinAnim() { return isFinAnim; }
	_bool Get_isEverEndAinm() { return m_isEverEndAinm; }

	void Set_isFinAnim(_bool b) { isFinAnim = b; }
	void Set_isEverEndAinm(_bool b) { m_isEverEndAinm = b; }

private:
    _uint Enter_Func_Cnt = 0;
    _bool isFinAnim = FALSE;
    CGameObject* m_pPlayer = nullptr;
    _float m_fTimer = 0.f;
    _bool m_isEverEndAinm = FALSE;

private:
    HRESULT     Add_Component()override;

public:
    static CArchvileBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CArchvileBullet* Clone() override;
    virtual CArchvileBullet* Clone(CGameObject* _Owner)override;



private:


private:
    virtual void        Free()override;

};

