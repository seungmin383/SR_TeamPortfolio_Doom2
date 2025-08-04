#include "CVIBuffer.h"

CVIBuffer::CVIBuffer()
	: m_dwVtxSize(0),
	m_dwVtxCnt(0),
	m_dwTriCnt(0),
	m_dwFVF(0),
	m_pVB(nullptr),
	m_pIB(nullptr),
	m_dwIdxSize(0)

{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev),
	m_dwVtxSize(0),
	m_dwVtxCnt(0),
	m_dwTriCnt(0),
	m_dwFVF(0),
	m_pVB(nullptr),
	m_pIB(nullptr),
	m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs),
	m_dwVtxSize(rhs.m_dwVtxSize),
	m_dwVtxCnt(rhs.m_dwVtxCnt),
	m_dwTriCnt(rhs.m_dwTriCnt),
	m_dwFVF(rhs.m_dwFVF),
	m_pVB(rhs.m_pVB),
	m_pIB(rhs.m_pIB),
	m_dwIdxSize(rhs.m_dwIdxSize),
	m_IdxFmt(rhs.m_IdxFmt)
{
	m_pVB->AddRef();
	m_pIB->AddRef();
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs, _bool isSpriteTex)
	: CComponent(rhs),
	m_dwVtxSize(rhs.m_dwVtxSize),
	m_dwVtxCnt(rhs.m_dwVtxCnt),
	m_dwTriCnt(rhs.m_dwTriCnt),
	m_dwFVF(rhs.m_dwFVF),
	m_pVB(nullptr), //버텍스 버퍼는 복사하지 않는다
	m_pIB(rhs.m_pIB),
	m_dwIdxSize(rhs.m_dwIdxSize),
	m_IdxFmt(rhs.m_IdxFmt)

{
	FAILED(m_pGraphicDev->
		CreateVertexBuffer(
			m_dwVtxCnt * m_dwVtxSize,
			0,
			m_dwFVF,
			D3DPOOL_MANAGED,
			&m_pVB,
			NULL));

	VTXTEX* pRhsVertex = NULL;
	VTXTEX* pVertex = NULL;

	rhs.m_pVB->Lock(0, 0, (void**)&pRhsVertex, 0);
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = pRhsVertex[0].vPosition;
	pVertex[0].vTexUV = pRhsVertex[0].vTexUV;

	pVertex[1].vPosition = pRhsVertex[1].vPosition;
	pVertex[1].vTexUV = pRhsVertex[1].vTexUV;

	pVertex[2].vPosition = pRhsVertex[2].vPosition;
	pVertex[2].vTexUV = pRhsVertex[2].vTexUV;

	pVertex[3].vPosition = pRhsVertex[3].vPosition;
	pVertex[3].vTexUV = pRhsVertex[3].vTexUV;
	m_pVB->Unlock();
	rhs.m_pVB->Unlock();


	m_pIB->AddRef();
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

	if (FAILED(m_pGraphicDev->
		CreateIndexBuffer(
			m_dwIdxSize * m_dwTriCnt,
			0,
			m_IdxFmt,
			D3DPOOL_MANAGED,
			&m_pIB,
			NULL)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Render_Buffer()
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

	m_pGraphicDev->SetFVF(m_dwFVF);

	m_pGraphicDev->SetIndices(m_pIB);

	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
	//m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwTriCnt);
}

void CVIBuffer::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CComponent::Free();
}
