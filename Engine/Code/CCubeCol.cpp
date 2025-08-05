#include "CCubeCol.h"

CCubeCol::CCubeCol()
{
}

CCubeCol::CCubeCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CCubeCol::CCubeCol(const CCubeCol& rhs)
	: CVIBuffer(rhs)
{
}

CCubeCol::~CCubeCol()
{
}

HRESULT CCubeCol::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 8;
	m_dwTriCnt = 12;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = { -1.f, 1.f, -1.f };
	pVertex[0].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[1].vPosition = { 1.f, 1.f, -1.f };
	pVertex[1].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[2].vPosition = { 1.f, -1.f, -1.f };
	pVertex[2].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[3].vPosition = { -1.f, -1.f, -1.f };
	pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//
	pVertex[4].vPosition = { -1.f, 1.f, 1.f };
	pVertex[4].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[5].vPosition = { 1.f, 1.f, 1.f };
	pVertex[5].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[6].vPosition = { 1.f, -1.f, 1.f };
	pVertex[6].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	pVertex[7].vPosition = { -1.f, -1.f, 1.f };
	pVertex[7].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	m_pVB->Unlock();

	INDEX32* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	pIndex[2]._0 = 1;
	pIndex[2]._1 = 5;
	pIndex[2]._2 = 6;

	pIndex[3]._0 = 1;
	pIndex[3]._1 = 6;
	pIndex[3]._2 = 2;

	pIndex[4]._0 = 4;
	pIndex[4]._1 = 0;
	pIndex[4]._2 = 3;

	pIndex[5]._0 = 4;
	pIndex[5]._1 = 3;
	pIndex[5]._2 = 7;

	pIndex[6]._0 = 5;
	pIndex[6]._1 = 4;
	pIndex[6]._2 = 7;

	pIndex[7]._0 = 5;
	pIndex[7]._1 = 7;
	pIndex[7]._2 = 6;

	pIndex[8]._0 = 5;
	pIndex[8]._1 = 1;
	pIndex[8]._2 = 0;

	pIndex[9]._0 = 5;
	pIndex[9]._1 = 0;
	pIndex[9]._2 = 4;

	pIndex[10]._0 = 7;
	pIndex[10]._1 = 3;
	pIndex[10]._2 = 2;

	pIndex[11]._0 = 7;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 6;
	m_pIB->Unlock();

	return S_OK;
}

void CCubeCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CCubeCol::Clone()
{
	return new CCubeCol(*this);
}

CCubeCol* CCubeCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCubeCol* pCubeCol = new CCubeCol(pGraphicDev);

	if (FAILED(pCubeCol->Ready_Buffer()))
	{
		Safe_Release(pCubeCol);
		MSG_BOX("CubeTex Create Failed");
		return nullptr;
	}

	return pCubeCol;
}

void CCubeCol::Free()
{
	CVIBuffer::Free();
}
