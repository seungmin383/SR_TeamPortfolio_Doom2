#include "CTriCol.h"

CTriCol::CTriCol()
{
}

CTriCol::CTriCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTriCol::CTriCol(const CTriCol& rhs)
	: CVIBuffer(rhs)
{
}

CTriCol::~CTriCol()
{
}

HRESULT CTriCol::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 3;
	m_dwTriCnt = 1;
	m_dwFVF = FVF_COL;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	return E_NOTIMPL;
}

void CTriCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CTriCol::Clone()
{
	return new CTriCol(*this);
}

CTriCol* CTriCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTriCol* pTriCol = new CTriCol(pGraphicDev);

	if (FAILED(pTriCol->Ready_Buffer()))
	{
		Safe_Release(pTriCol);
		MSG_BOX("TriCol Create Failed");
		return nullptr;
	}

	return pTriCol;
}

void CTriCol::Free()
{
	CVIBuffer::Free();
}
