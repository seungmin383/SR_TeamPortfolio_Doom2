#include "pch.h"
#include "CVerticalMovingObject.h"



CVerticalMovingObject::CVerticalMovingObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CVerticalMovingObject::CVerticalMovingObject(const CGameObject& rhs)
    : CGameObject(rhs)
{
}

CVerticalMovingObject::~CVerticalMovingObject()
{
}

HRESULT CVerticalMovingObject::Ready_GameObject()
{
    return E_NOTIMPL;
}

_int CVerticalMovingObject::Update_GameObject(const _float& fTimeDelta)
{
    return _int();
}

void CVerticalMovingObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CVerticalMovingObject::Render_GameObject(const _float& fTimeDelta)
{
}

void CVerticalMovingObject::OnCollision(CCollider* _pOther)
{
}

void CVerticalMovingObject::OnCollisionEnter(CCollider* _pOther)
{
    Moving();
    // 몇 초 후 움직임
}

void CVerticalMovingObject::OnCollisionExit(CCollider* _pOther)
{
}

void CVerticalMovingObject::Moving()
{
}

HRESULT CVerticalMovingObject::Add_Component()
{
   // 텍스처, 콜라이더, transform, 
}

void CVerticalMovingObject::Set_OnTerrain()
{
}

void CVerticalMovingObject::Bill_Board()
{
}

void CVerticalMovingObject::DoorOpening(const _float& fTimeDelta)
{
}



CVerticalMovingObject* CVerticalMovingObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

CVerticalMovingObject* CVerticalMovingObject::Clone()
{
    return nullptr;
}


void CVerticalMovingObject::Free()
{
    Engine::CGameObject::Free();
}