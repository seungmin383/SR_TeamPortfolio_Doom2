#include <fstream>
#include "CTerrainTexSector.h"

CTerrainTexSector::CTerrainTexSector()
{
}

CTerrainTexSector::CTerrainTexSector(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev),
	m_pPos(nullptr)
{
}

CTerrainTexSector::CTerrainTexSector(const CTerrainTexSector& rhs)
	: CVIBuffer(rhs),
	m_pPos(rhs.m_pPos)
{
}

CTerrainTexSector::~CTerrainTexSector()
{
}

HRESULT CTerrainTexSector::Ready_Buffer(const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv,
	vector<_vec3> vertices)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwFVF = FVF_TEX;

	_float colCnt = dwCntX - 1;
	_float rowCnt = dwCntZ - 1;

	m_VtxCntX = dwCntX;
	m_VtxCntZ = dwCntZ;
	m_VtxItv = dwVtxItv;

	Safe_Delete_Array(m_pPos);

	m_pPos = new _vec3[m_dwVtxCnt];

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	_ulong dwIndex(0);
	for (size_t i = 0; i < dwCntZ; i++)
	{
		for (size_t j = 0; j < dwCntX; j++)
		{
			dwIndex = j + (i * dwCntX);

			pVertex[dwIndex].vPosition = { vertices[dwIndex].x * dwVtxItv,
											vertices[dwIndex].y* dwVtxItv,
											vertices[dwIndex].z* dwVtxItv };

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };

			pVertex[dwIndex].vTexUV = { (_float)(dwCntX - j) / (dwCntX - 1) * colCnt,
										(_float)i / (dwCntZ - 1) * rowCnt };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;
		}
	}

	INDEX32* pIndex = NULL;
	_ulong dwTriCnt(0);
	_vec3 vDst, vSrc, vNormal;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	for (size_t i = 0; i < dwCntZ - 1; i++)
	{
		for (size_t j = 0; j < dwCntX - 1; j++)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;
		}
	}

	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
	{
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

void CTerrainTexSector::Render_Buffer()
{
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	CVIBuffer::Render_Buffer();

	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	//m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
}

CComponent* CTerrainTexSector::Clone()
{
	return new CTerrainTexSector(*this);
}

CTerrainTexSector* CTerrainTexSector::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	vector<_vec3> vertices,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _float& dwVtxItv)
{
	CTerrainTexSector* pTerrainTex = new CTerrainTexSector(pGraphicDev);

	if (FAILED(pTerrainTex->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv, vertices)))
	{
		Safe_Release(pTerrainTex);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pTerrainTex;
}

CTerrainTexSector* CTerrainTexSector::Create_Default(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxItv)
{
	CTerrainTexSector* pTerrainTex = new CTerrainTexSector(pGraphicDev);

	if (FAILED(pTerrainTex->Ready_Buffer_Default(dwCntX, dwCntZ, dwVtxItv)))
	{
		Safe_Release(pTerrainTex);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pTerrainTex;
}

HRESULT CTerrainTexSector::Ready_Buffer_Default(const _ulong& dwCntX, const _ulong& dwCntZ, const _float& dwVtxItv)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwFVF = FVF_TEX;

	_float colCnt = dwCntX - 1;
	_float rowCnt = dwCntZ - 1;

	m_VtxCntX = dwCntX;
	m_VtxCntZ = dwCntZ;
	m_VtxItv = dwVtxItv;

	Safe_Delete_Array(m_pPos);

	m_pPos = new _vec3[m_dwVtxCnt];

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	_ulong dwIndex(0);
	for (size_t i = 0; i < dwCntZ; i++)
	{
		for (size_t j = 0; j < dwCntX; j++)
		{
			dwIndex = j + (i * dwCntX);

			pVertex[dwIndex].vPosition = { (_float)j * dwVtxItv,
											0.f,
											(_float)i* dwVtxItv };

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };

			pVertex[dwIndex].vTexUV = { (_float)j / (dwCntX - 1) * 20.f,
										(_float)i / (dwCntZ - 1) * 20.f };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;
		}
	}

	INDEX32* pIndex = NULL;
	_ulong dwTriCnt(0);
	_vec3 vDst, vSrc, vNormal;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	for (size_t i = 0; i < dwCntZ - 1; i++)
	{
		for (size_t j = 0; j < dwCntX - 1; j++)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;
		}
	}

	for (_ulong i = 0; i < m_dwVtxCnt; ++i)
	{
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

void CTerrainTexSector::Free()
{
	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pPos);
	}

	CVIBuffer::Free();
}
