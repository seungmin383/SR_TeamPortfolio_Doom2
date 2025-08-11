#include "CRectSpriteTex.h"

CRectSpriteTex::CRectSpriteTex()
{
}

CRectSpriteTex::CRectSpriteTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRectSpriteTex::CRectSpriteTex(const CRectSpriteTex& rhs)
	: CVIBuffer(rhs, true)
{
}

CRectSpriteTex::~CRectSpriteTex()
{
}

HRESULT CRectSpriteTex::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].vTexUV = { 0.02f, 0.02f };

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].vTexUV = { 0.98f, 0.02f };

	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].vTexUV = { 0.98f, 0.98f };

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].vTexUV = { 0.02f, 0.98f };
	m_pVB->Unlock();

	INDEX32* pIndex = NULL;

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

void CRectSpriteTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

void CRectSpriteTex::Set_UVCoord(const _uint& _iCurFrame, const _float& _fU, const _uint& _iCurState, const _float& _fV, _bool isReverse)
{
	VTXTEX* pVertex = NULL;

	_float reversePadding(1.f);
	if (isReverse)
		reversePadding = -1.f;

	_float startU = _iCurFrame * _fU;
	_float endU = startU + _fU;

	_float startV = _iCurState * _fV;
	_float endV = startV + _fV;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	pVertex[0].vTexUV = { startU * reversePadding, startV };

	pVertex[1].vTexUV = { endU * reversePadding, startV };

	pVertex[2].vTexUV = { endU * reversePadding, endV};

	pVertex[3].vTexUV = { startU * reversePadding, endV};
	m_pVB->Unlock();
}

CComponent* CRectSpriteTex::Clone()
{
	return new CRectSpriteTex(*this);
}

CRectSpriteTex* CRectSpriteTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRectSpriteTex* pRectSpriteTex = new CRectSpriteTex(pGraphicDev);

	if (FAILED(pRectSpriteTex->Ready_Buffer()))
	{
		Safe_Release(pRectSpriteTex);
		MSG_BOX("RectTex Create Failed");
		return nullptr;
	}

	return pRectSpriteTex;
}

void CRectSpriteTex::Free()
{
	CVIBuffer::Free();
}
