#pragma once
#include "CCamera.h"
#include "CPrototypeManager.h"

class CDynamicCamera :
    public CCamera
{
private:
    explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CDynamicCamera();

public:
    HRESULT Ready_GameObject(const _vec3* pEye,
        const _vec3* pAt,
        const _vec3* pUp,
        const _float& fFov,
        const _float& fAspect,
        const _float& fNear,
        const _float& fFar);

    virtual         _int        Update_GameObject(const _float& fTimeDelta);
    virtual         void        LateUpdate_GameObject(const _float& fTimeDelta);
    virtual         void        Render_GameObject(const _float& fTimeDelta);

public:
    _vec3       Get_Pos()   { return m_vEye; }
    _vec3       Get_At()    { return m_vAt; }
    _vec3       Get_Up()    { return m_vUp; }
    // 마우스 x축 이동량을 저장
    static _long s_lMouseMoveX;

    _int Get_Hp() { return m_iHp; }
    void Set_Hp(_int i) { m_iHp = i; }

    _int Get_Armor() { return m_iArmor; }
    void Set_Armor(_int i) { m_iArmor = i; }

private:
    HRESULT     Add_Component();
    void        Key_Input(const _float& fTimeDelta);
    void        Mouse_Move();
    void        Mouse_Fix();
    void        Set_OnTerrain();

private:
    Engine::CCalculator*    m_pCalculatorCom;

private:
    _int        m_iHp;
    _int        m_iArmor;
    
    _float      m_fSpeed;
    _bool       m_bFix;
    _bool       m_bCheck;

public:
    static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const _vec3* pEye,
        const _vec3* pAt,
        const _vec3* pUp,
        const _float& fFov = D3DXToRadian(60.f),
        const _float& fAspect = (_float)WINCX / WINCY,
        const _float& fNear = 0.1f,
        const _float& fFar = 1000.f);

    // Scale = 1/tanf(fFov * 0.5f)
    // 1/(tanf(fFov * 0.5f)) = fNear
    // d:-Zv = Yn : Yv
    // 
    // //투영평면이 정사각형으로 표현되는 경우
    // Xn = d * Xv / -Zv
    // Yn = d * Yv / -Zv 
    //
    // //최종 투영 변환 공식
    // Xn = d * Xv / -Zv * fAspect
    // Yn = d * Yv / -Zv * fAspect 


private:
    virtual void        Free();
};

