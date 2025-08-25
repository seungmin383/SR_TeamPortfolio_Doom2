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
	m_bHit(false),
	m_fHitDelay(0.5f),
	m_fHitTime(0.f),
	m_pOwner(nullptr), m_pLine(nullptr),
	m_bActive(true)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs),
	m_iID(g_iNextID++),
	m_iCol(rhs.m_iCol),
	m_eColLayerId(rhs.m_eColLayerId),
	m_iColMask(rhs.m_iColMask),
	m_bHit(rhs.m_bHit),
	m_fHitDelay(rhs.m_fHitDelay),
	m_fHitTime(rhs.m_fHitTime),
	m_pOwner(rhs.m_pOwner), m_pLine(nullptr),
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

	if (FAILED(D3DXCreateLine(m_pGraphicDev, &m_pLine)))
		return E_FAIL;

	return S_OK;
}

void CCollider::Late_Update()
{
	m_pOwner->Get_Transform()->Get_Pos(&m_vPos);
	m_vScaleHalf = m_vScale * 0.5f;
	m_vFinalPos = m_vPos + m_vOffsetPos;

	Update_Box();

	if (m_bHit)
	{
		m_fHitTime += 0.05f;
		if (m_fHitTime > m_fHitDelay)
		{
			m_bHit = false;
			m_fHitTime = 0.f;
		}
	}

	assert(0 <= m_iCol);
}

void CCollider::Render(_bool _colRend)
{
	if (!_colRend || !m_bActive || m_pLine == nullptr)
		return;

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	matView = matView * matProj;
	
	//LPD3DXLINE pLine;
	//D3DXCreateLine(m_pGraphicDev, &pLine);

	_vec3 vpoints[8] =
	{ m_ColliderBox[0], m_ColliderBox[1],
		m_ColliderBox[2], m_ColliderBox[3],
		m_ColliderBox[4], m_ColliderBox[5],
		m_ColliderBox[6], m_ColliderBox[7] };

	D3DCOLOR color = (m_iCol || m_bHit) ? D3DCOLOR_RGBA(255, 0, 0, 255) : D3DCOLOR_RGBA(0, 255, 0, 255);

	_vec3 vline[2];
	m_pLine->Begin();
	vline[0] = vpoints[0]; vline[1] = vpoints[1];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[0]; vline[1] = vpoints[3];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[0]; vline[1] = vpoints[4];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[5]; vline[1] = vpoints[4];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[5]; vline[1] = vpoints[1];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[5]; vline[1] = vpoints[6];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[7]; vline[1] = vpoints[3];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[7]; vline[1] = vpoints[4];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[7]; vline[1] = vpoints[6];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[2]; vline[1] = vpoints[3];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[2]; vline[1] = vpoints[1];
	m_pLine->DrawTransform(vline, 2, &matView, color);

	vline[0] = vpoints[2]; vline[1] = vpoints[6];
	m_pLine->DrawTransform(vline, 2, &matView, color);
	m_pLine->End();
}

CComponent* CCollider::Clone()
{
	return new CCollider(*this);
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGarphicDev)
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

void CCollider::HitRay(CCollider* _pOther)
{
	if (!m_bActive || m_bHit)
		return;

	m_bHit = true;
	m_pOwner->OnCollisionEnter(_pOther);
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
	Safe_Release(m_pLine);
	CComponent::Free();
}
