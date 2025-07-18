#include "CVIBuffer.h"

CVIBuffer::CVIBuffer()
	: m_dwVtxSize(0),
	m_dwVtxCnt(0),
	m_dwTriCnt(0),
	m_dwFVF(0),
	m_pVB(nullptr)

{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_dwVtxSize(0),
	m_dwVtxCnt(0),
	m_dwTriCnt(0),
	m_dwFVF(0),
	m_pVB(nullptr)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs),
	m_dwVtxSize(rhs.m_dwVtxSize),
	m_dwVtxCnt(rhs.m_dwVtxCnt),
	m_dwTriCnt(rhs.m_dwTriCnt),
	m_dwFVF(rhs.m_dwFVF),
	m_pVB(rhs.m_pVB)
{
}

CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::Ready_Buffer()
{
	//D3DPOOL_DEFAULT = 0,
	//D3DPOOL_MANAGEED = 1,
	//D3DPOOL_SYSTEMMEM = 2,
	//D3DPOOL_SCRATCH = 3

	if (FAILED(m_pGraphicDev->
		CreateVertexBuffer(
			m_dwVtxCnt * m_dwVtxSize,
			0,
			m_dwFVF,
			D3DPOOL_MANAGED,
			&m_pVB,
			NULL)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Render_Buffer()
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

	m_pGraphicDev->SetFVF(m_dwFVF);

	m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriCnt);
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);

	CComponent::Free();
}
