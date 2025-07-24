#include "CRectTex.h"

CRectTex::CRectTex()
{
}

CRectTex::CRectTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRectTex::CRectTex(const CRectTex& rhs)
	: CVIBuffer(rhs)
{
}

CRectTex::~CRectTex()
{
}

HRESULT CRectTex::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = { -1.f, 0.f, 1.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 1.f, 0.f, 1.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 1.f, 0.f, -1.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -1.f, 0.f, -1.f };
	pVertex[3].vTexUV = { 0.f, 1.f };
	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;
	m_pIB->Unlock();

	return S_OK;
}

void CRectTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CRectTex::Clone()
{
	return new CRectTex(*this);
}

CRectTex* CRectTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRectTex* pRectTex = new CRectTex(pGraphicDev);

	if (FAILED(pRectTex->Ready_Buffer()))
	{
		Safe_Release(pRectTex);
		MSG_BOX("RectTex Create Failed");
		return nullptr;
	}

	return pRectTex;
}

void CRectTex::Free()
{
	CVIBuffer::Free();
}
