#include <fstream>
#include "CTerrainTex.h"

CTerrainTex::CTerrainTex()
{
}

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	: CVIBuffer(rhs)
{
}

CTerrainTex::~CTerrainTex()
{
}

HRESULT CTerrainTex::Ready_Buffer(const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwFVF = FVF_TEX;

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

			pVertex[dwIndex].vPosition = { (_float)j * dwVtxItv, 0.f, (_float)i * dwVtxItv };

			pVertex[dwIndex].vTexUV = { (_float)j / (dwCntX - 1), (_float)i / (dwCntZ - 1) };
		}
	}
	m_pVB->Unlock();

	INDEX32* pIndex = NULL;
	_ulong dwTriCnt(0);
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);
	for (size_t i = 0; i < dwCntZ - 1; i++)
	{
		for (size_t j = 0; j < dwCntX - 1; j++)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;
			dwTriCnt++;

			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;
			dwTriCnt++;
		}
	}
	m_pIB->Unlock();

	return S_OK;
}

void CTerrainTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CComponent* CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

CTerrainTex* CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv)
{
	CTerrainTex* pTerrainTex = new CTerrainTex(pGraphicDev);

	if (FAILED(pTerrainTex->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
	{
		Safe_Release(pTerrainTex);
		MSG_BOX("TileGround Create Failed");
		return nullptr;
	}

	return pTerrainTex;
}

_bool CTerrainTex::ReadRawFile(string _strFileName)
{
	vector<_byte> in(m_iNumVertices);

	ifstream inFile(_strFileName.c_str(), ios_base::binary);

	if (inFile.peek() == char_traits<char>::eof())
		return false;

	inFile.read(
		(char*)&in[0],
		in.size());

	inFile.close();

	m_vecHeightmap.resize(m_iNumVertices);

	for (_int i = 0; i < in.size(); i++)
		m_vecHeightmap[i] = in[i];

	return true;
}

int CTerrainTex::Get_HeightMapEntry(int _row, int _col)
{
	return m_vecHeightmap[_row * m_iNumVertices + _col];
}

void CTerrainTex::Set_HeightMapEntry(int _row, int _col, int _value)
{
	m_vecHeightmap[_row * m_iNumVertsPerRow + _col] = _value;
}

void CTerrainTex::Free()
{
	CVIBuffer::Free();
}
