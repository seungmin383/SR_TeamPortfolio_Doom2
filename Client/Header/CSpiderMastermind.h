#pragma once
#include "CMonsterBase.h"
#include "CPrototypeManager.h"

class CSpiderMastermind :public CMonsterBase
{
private:
    explicit CSpiderMastermind(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CSpiderMastermind(const CMonsterBase& rhs);
    virtual ~CSpiderMastermind();

public:
    virtual     HRESULT         Ready_GameObject();
    virtual     _int            Update_GameObject(const _float& fTimeDelta);
    virtual     void            LateUpdate_GameObject(const _float& fTimeDelta);
    virtual     void            Render_GameObject(const _float& fTimeDelta);


    virtual void         Attack(const _float fTimeDelta)override;
    
    virtual void Check_Anim_Fin(const _float& fTimeDelta);

    //난수를 이용해서 패턴 규칙 만들기
    virtual void Calc_Pattern();

    virtual void Fire(const _float fTimeDelta);

    virtual void Reset_Pattern1(const _float fTimeDelta);
    virtual void Reset_Pattern2(const _float fTimeDelta);
    virtual void Reset_Pattern3(const _float fTimeDelta);
    virtual void Reset_Pattern4(const _float fTimeDelta);

    virtual void Pattern1(const _float fTimeDelta);
    virtual void Pattern2(const _float fTimeDelta);
    virtual void Pattern3(const _float fTimeDelta);
    virtual void Pattern4(const _float fTimeDelta);

public://FUNCTION   
	virtual void Pattern_Timer(const _float fTimeDelta);
public://GETTER SETTER
	_float Get_PatternTimer() { return m_fPattern_Timer; }
	_float Get_PatternDelay() { return m_fPattern_Delay; }
	_float Get_EntPatternNumber() { return m_iEnt_Pattern_Number; }

	void Set_PatternTimer(_float fTime) { m_fPattern_Timer = fTime; }
	void Set_PatternDelay(_float fTime) { m_fPattern_Delay = fTime; }
	void Set_EntPatternNumber(_int iNum) { m_iEnt_Pattern_Number = iNum; }
private://VARIABLE
	_float m_fPattern_Timer = 0.f; //패턴 타이머
	_float m_fPattern_Delay = 5.f;	//패턴이 바뀌는 시간
	_int m_iEnt_Pattern_Number = 0; //패턴함수 실행 횟수
	_bool m_isAwake = FALSE;

public:
    virtual void Awake_Pattern(_int _Pattern);

public:
    virtual void Update_Pattern(const _float& fTimeDelta, _int _Pattern);

#pragma endregion

private:
    HRESULT     Add_Component()override;

    uint16_t  m_iPattern = 0;
    _int m_PattenArr[8] = {};
    _bool m_isPattern3On = FALSE;

    _int m_iFuncEntCnt = 0;
    _bool m_isFinAnim = FALSE;

	_vec3 m_vPatternLook = {};

public:
    static CSpiderMastermind* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual CSpiderMastermind* Clone() override;

private:
    virtual void        Free();




};

