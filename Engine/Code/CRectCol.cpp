#include "CRectCol.h"

CRectCol::CRectCol()
{
}

CRectCol::CRectCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRectCol::CRectCol(const CRectCol& rhs)
	: CVIBuffer(rhs)
{
}

CRectCol::~CRectCol()
{
}

HRESULT CRectCol::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_COL;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	
	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	
	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	
	pVertex[3].vPosition = { -1.f, 1.f, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	m_pVB->Unlock();

	WORD* indices = NULL;
	m_pIB->Lock(0, 0, (void**)&indices, 0);
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;
	m_pIB->Unlock();

	return S_OK;
}

void CRectCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CRectCol::Clone()
{
	return new CRectCol(*this);
}

CRectCol* CRectCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRectCol* pRectCol = new CRectCol(pGraphicDev);

	if (FAILED(pRectCol->Ready_Buffer()))
	{
		Safe_Release(pRectCol);
		MSG_BOX("RectCol Create Failed");
		return nullptr;
	}

	return pRectCol;
}

void CRectCol::Free()
{
	CVIBuffer::Free();
}
