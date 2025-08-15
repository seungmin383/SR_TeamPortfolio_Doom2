#include "pch.h"
#include "CTerrainSector.h"
#include "CTerrain.h"
#include "CRenderer.h"
#include <cmath>

CTerrainSector::CTerrainSector(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTerrainSector::CTerrainSector(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CTerrainSector::~CTerrainSector()
{
}

HRESULT CTerrainSector::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_Material()))
		return E_FAIL;

	return S_OK;
}

_int CTerrainSector::Update_GameObject(const _float& fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_REFLECT, this);

	return 0;
}

void CTerrainSector::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTerrainSector::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pMaterialCom->Set_Material();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CTerrainSector::Set_BufferCom(Engine::CTerrainTexSector* pBuffer)
{
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pBuffer });
}

void CTerrainSector::Mod_BufferCom(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices)
{
	m_pBufferCom->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv, vertices);
}

void CTerrainSector::Set_TextureCom(Engine::CTexture* pTexture)
{
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pTexture });
}

HRESULT CTerrainSector::Add_Component()
{
	CComponent* pComponent = NULL;

	//pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTexSector*>
	//	(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_TerrainTex"));
	//if (nullptr == pComponent)
	//	return E_FAIL;

	//m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	////Texture
	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
	//	(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_TerrainTexture"));
	//if (nullptr == pComponent)
	//	return E_FAIL;

	//m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pMaterialCom = dynamic_cast<Engine::CMaterial*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Material"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Material", pComponent });

	return S_OK;
}

HRESULT CTerrainSector::Ready_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pMaterialCom->Ready_Material(&tMtrl);

	return S_OK;
}

CTerrainSector* CTerrainSector::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrainSector* pTerrain = new CTerrainSector(pGraphicDev);

	return pTerrain;
}

CTerrainSector* CTerrainSector::Clone()
{
	CTerrainSector* pTerrain = new CTerrainSector(*this);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("Terrain Cretae Failed");
		return nullptr;
	}

	return pTerrain;
}

void CTerrainSector::Free()
{
	Engine::CGameObject::Free();
}
