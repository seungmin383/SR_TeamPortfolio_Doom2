#include "CCollider.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "assert.h"

UINT CCollider::g_iNextID = 0;

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_iID(g_iNextID++),
	m_iCol(0),
	m_eColLayerId(COL_END),
	m_iColMask(0),
	m_pOwner(nullptr),
	m_bActive(true)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_CCollider()
{
	return S_OK;
}

void CCollider::Late_Update()
{
	_vec3 vObjPos;
	static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Pos(&vObjPos);
	m_vFinalPos = vObjPos + m_vOffsetPos;
	
	assert(0 <= m_iCol);
}

void CCollider::Render()
{
}

void CCollider::OnCollision(CCollider* _pOther)
{
	m_pOwner->OnCollision(_pOther);
}

void CCollider::OnCollisionEnter(CCollider* _pOther)
{
	m_pOwner->OnCollisionEnter(_pOther);
	++m_iCol;
}

void CCollider::OnCollisionExit(CCollider* _pOther)
{
	m_pOwner->OnCollisionExit(_pOther);
	--m_iCol;
}
