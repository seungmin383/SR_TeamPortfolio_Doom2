#include "pch.h"
#include "CWeapon_BFG.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionManager.h"
#include "CTimerManager.h"
#include "CWeaponManager.h"

CWeapon_BFG::CWeapon_BFG(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWeapon_BFG::CWeapon_BFG(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CWeapon_BFG::~CWeapon_BFG()
{
}

HRESULT CWeapon_BFG::Ready_GameObject()
{
    return E_NOTIMPL;
}

_int CWeapon_BFG::Update_GameObject(const _float& fTimeDelta)
{
    return _int();
}

void CWeapon_BFG::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CWeapon_BFG::Render_GameObject(const _float& fTimeDelta)
{
}

void CWeapon_BFG::OnCollision(CCollider* _pOther)
{
}

void CWeapon_BFG::OnCollisionEnter(CCollider* _pOther)
{
}

void CWeapon_BFG::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CWeapon_BFG::Add_Component()
{
    return E_NOTIMPL;
}

void CWeapon_BFG::Set_OnTerrain()
{
}

void CWeapon_BFG::Bill_Board()
{
}

CWeapon_BFG* CWeapon_BFG::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

CWeapon_BFG* CWeapon_BFG::Clone()
{
    return nullptr;
}

void CWeapon_BFG::Free()
{
}
