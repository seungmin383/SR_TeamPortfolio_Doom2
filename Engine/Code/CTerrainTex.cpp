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

	m_hFile = CreateFile(L"../Bin/Resource/Texture/Terrain/Height1.bmp",
							GENERIC_READ,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							0);

	if (INVALID_HANDLE_VALUE == m_hFile)
		return E_FAIL;
	
	_ulong dwByte(0);

	ReadFile(m_hFile, &m_fh, sizeof(PBITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_ih, sizeof(PBITMAPINFOHEADER), &dwByte, NULL);

	_ulong* pPixel = new _ulong[m_ih.biWidth * m_ih.biHeight];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_ih.biWidth * m_ih.biHeight, &dwByte, NULL);

	CloseHandle(m_hFile);

	VTXTEX* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	_ulong dwIndex(0);
	for (size_t i = 0; i < dwCntZ; i++)
	{
		for (size_t j = 0; j < dwCntX; j++)
		{
			dwIndex = j + (i * dwCntX);

			pVertex[dwIndex].vPosition = { (_float)j * dwVtxItv,
											_float(pPixel[dwIndex] & 0x000000ff) / 20.f,
											(_float)i * dwVtxItv };

			pVertex[dwIndex].vTexUV = { (_float)j / (dwCntX - 1) * 20.f,
										(_float)i / (dwCntZ - 1) * 20.f };

			m_arrHeightmap.push_back(_float(pPixel[dwIndex] & 0x000000ff) / 20.f);
		}
	}

	Safe_Delete_Array(pPixel);
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

_float CTerrainTex::Get_HeightmapEntry(const _int row, const _int col)
{
	VTXTEX* pVertex = NULL;
	_float height;

	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, 0)))
		return E_FAIL;

	height = pVertex[row * VTXCNTX + col].vPosition.y;

	m_pVB->Unlock();

	//return m_arrHeightmap[row * VTXCNTX + col];

	return height;
}

_float CTerrainTex::Get_Height(const _float& _fX, const _float& _fZ)
{
	//지형의 시작점을 원점으로 이동하는 변환
	//xz-평면 상에서 이동
	_float height(0);

	if ((_fX < 0 || _fZ < 0) || (_fX > VTXCNTX || _fZ > VTXCNTZ))
		return height;

	_float x = _fX;
	_float z = _fZ;

	//셀 간격을 1로 만드는 변환
	//변환의 배율을 낮춘다
	x /= (_float)VTXITV;
	z /= (_float)VTXITV;

	_float col = ::floorf(x);
	_float row = ::floorf(z);

	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D

	_float A = Get_HeightmapEntry(row, col);
	_float B = Get_HeightmapEntry(row, col + 1);
	_float C = Get_HeightmapEntry(row + 1, col);
	_float D = Get_HeightmapEntry(row, col + 1);

	//row와 col은 현재 위치의 왼쪽 상단 버텍스 위치를 나타낸다
	//x와 z를 원점으로 이동시킨다
	//dz < 1.f-dx면 위쪽 삼각형 아니라면 아래쪽 삼각형
	_float dx = x - col;
	_float dz = z - row;

	auto Lerp = [&](_float a, _float b, _float t) { return a - (a * t) + (b * t); };

	if (dz < 1.f - dx)
	{
		_float uy = B - A; // A->B
		_float vy = C - A; // A->C
		height = D + Lerp(0.f, uy, dx) + Lerp(0.f, vy, dz);
	}
	else
	{
		_float uy = C - D; // D->C
		_float vy = B - D; // D->B
		height = D + Lerp(0.f, uy, 1.f - dx) + Lerp(0.f, vy, 1.f - dz);
	}

	return height;
}

void CTerrainTex::Free()
{
	CVIBuffer::Free();
}
