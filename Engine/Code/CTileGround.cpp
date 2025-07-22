#include "CTileGround.h"

CTileGround::CTileGround()
{
}

CTileGround::CTileGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTileGround::CTileGround(const CTileGround& rhs)
	: CVIBuffer(rhs)
{
}

CTileGround::~CTileGround()
{
}

HRESULT CTileGround::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 129 * 129;
	m_dwTriCnt = 16385;//128 * 2 * 128;
	m_dwFVF = FVF_COL;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	for (size_t i = 0; i < 129; i++)
	{
		for (size_t j = 0; j < 129; j++)
		{
			pVertex[j + (i * 129)].vPosition = { (float)j, (float)i, 0.f };
		}
	}
	m_pVB->Unlock();

	WORD* indices = NULL;
	m_pIB->Lock(0, 0, (void**)&indices, 0);
	int index = 0;
	for (size_t i = 0; i < 128; i++)
	{
		for (size_t j = 0; j < 128; j++)
		{
			indices[index] = j + (i * 129);;
			indices[index + 1] = j + ((1 + i) * 129);
			indices[index + 2] = j + (i * 129) + 1;

			indices[index + 3] = j + (i * 129) + 1;
			indices[index + 4] = j + ((1 + i) * 129);
			indices[index + 5] = j + ((1 + i) * 129) + 1;
			index += 6;
		}
	}
	m_pIB->Unlock();

	return S_OK;
}

void CTileGround::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CTileGround::Clone()
{
	return new CTileGround(*this);
}

CTileGround* CTileGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTileGround* pTileGround = new CTileGround(pGraphicDev);

	if (FAILED(pTileGround->Ready_Buffer()))
	{
		Safe_Release(pTileGround);
		MSG_BOX("TileGround Create Failed");
		return nullptr;
	}

	return pTileGround;
}

void CTileGround::Free()
{
	CVIBuffer::Free();
}
