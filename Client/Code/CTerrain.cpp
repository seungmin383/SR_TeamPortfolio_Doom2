#include "pch.h"
#include "CTerrain.h"
#include "CRenderer.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrain::CTerrain(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTerrain::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//_float CTerrain::Get_Height(const _float& _fX, const _float& _fZ)
//{
//	//지형의 시작점을 원점으로 이동하는 변환
//	//xz-평면 상에서 이동
//	int width = VTXCNTX * VTXITV;
//	int height = VTXCNTZ * VTXITV;
//	_float x = ((_float)width / 2.f) + _fX;
//	_float z = ((_float)height / 2.f) + _fZ;
//
//	//셀 간격을 1로 만드는 변환
//	//변환의 배율을 낮춘다
//	x /= (_float)VTXITV;
//	z /= (_float)VTXITV;
//
//	_float col = ::floorf(x);
//	_float row = ::floorf(z);
//
//	//  A   B
//	//  *---*
//	//  | / |
//	//  *---*  
//	//  C   D
//
//	/*_float A = m_pBufferCom->Get_Height(row, col);
//	_float B = m_pBufferCom->Get_Height(row, col+1);
//	_float C = m_pBufferCom->Get_Height(row+1, col);
//	_float D = m_pBufferCom->Get_Height(row, col+1);*/
//
//	//row와 col은 현재 위치의 왼쪽 상단 버텍스 위치를 나타낸다
//	//x와 z를 원점으로 이동시킨다
//	//dz < 1.f-dx면 위쪽 삼각형 아니라면 아래쪽 삼각형
//	_float dx = x - col;
//	_float dz = z - row;
//
//	if (dz < 1.f - dx)
//	{
//		//_float uy = B - A; // A->B
//		//_float vy = C - A; // A->C
//	}
//
//	return _float();
//}

HRESULT CTerrain::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_TerrainTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_Prototype(L"Proto_TerrainTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pTerrain = new CTerrain(pGraphicDev);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("Terrain Cretae Failed");
		return nullptr;
	}

	return pTerrain;
}

void CTerrain::Free()
{
	Engine::CGameObject::Free();
}
