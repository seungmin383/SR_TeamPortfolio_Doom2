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

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs),
	m_iID(g_iNextID++),
	m_iCol(rhs.m_iCol),
	m_eColLayerId(rhs.m_eColLayerId),
	m_iColMask(rhs.m_iColMask),
	m_pOwner(rhs.m_pOwner),
	m_bActive(rhs.m_bActive)

{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_CCollider(CGameObject* _pObj, _vec3 _vPos, _vec3 _vScale, _vec3 _vOffset,
	COLLAYERID _eColLayerId, _int _iColMask)
{
	Set_Owner(_pObj);
	m_vPos = _vPos;
	m_vScale = _vScale;
	m_vScaleHalf = m_vScale * 0.5f;
	m_vOffsetPos = _vOffset;
	m_vFinalPos = m_vPos + m_vOffsetPos;
	m_eColLayerId = _eColLayerId;
	m_iColMask = _iColMask;

	return S_OK;
}

void CCollider::Late_Update()
{
	m_pOwner->Get_Transform()->Get_Pos(&m_vPos);
	m_vFinalPos = m_vPos + m_vOffsetPos;
	
	Update_Box();

	assert(0 <= m_iCol);
}

void CCollider::Render()
{
}

CComponent* CCollider::Clone()
{
	return new CCollider(*this);
}

CCollider* CCollider::Cretae(LPDIRECT3DDEVICE9 pGarphicDev)
{
	CCollider* pCollider = new CCollider(pGarphicDev);

	return pCollider;
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

void CCollider::Update_Box()
{
	m_pOwner->Get_Transform()->Get_Pos(&m_vPos);

	m_ColliderBox[0] = { m_vPos.x - m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y + m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z - m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[1] = { m_vPos.x + m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y + m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z - m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[2] = { m_vPos.x + m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y - m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z - m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[3] = { m_vPos.x - m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y - m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z - m_vScaleHalf.z + m_vOffsetPos.z };

	m_ColliderBox[4] = { m_vPos.x - m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y + m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z + m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[5] = { m_vPos.x + m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y + m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z + m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[6] = { m_vPos.x + m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y - m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z + m_vScaleHalf.z + m_vOffsetPos.z };
	m_ColliderBox[7] = { m_vPos.x - m_vScaleHalf.x + m_vOffsetPos.x, m_vPos.y - m_vScaleHalf.y + m_vOffsetPos.y, m_vPos.z + m_vScaleHalf.z + m_vOffsetPos.z };
}

void CCollider::Free()
{
	CComponent::Free();
}
