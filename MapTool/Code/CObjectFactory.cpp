#include "pch.h"
#include "CObjectFactory.h"
#include "CManagement.h"
#include "CTerrainSector.h"
#include "CTextureManager.h"

IMPLEMENT_SINGLETON(CObjectFactory)

CObjectFactory::CObjectFactory()
{
}

CObjectFactory::~CObjectFactory()
{
	Free();
}

void CObjectFactory::Set_Dev(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();
}

CGameObject* CObjectFactory::Create_Object(CGameObject* pObj, OBJID eObjId, _vec3 vPos, _tchar* texTag)
{
	return nullptr;
}

CGameObject* CObjectFactory::Create_Terrain(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos)
{
	auto sectorBuffer = Engine::CTerrainTexSector::Create(m_pGraphicDev, vertices, dwCntX, dwCntZ, dwVtxItv);
	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texTag);

	auto terrainSector = dynamic_cast<CTerrainSector*>(CPrototypeManager::GetInstance()->GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::TERRAINSECTOR));

	terrainSector->Set_BufferCom(sectorBuffer);
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));
	
	terrainSector->Get_Transform()->Set_Pos(pos.x, pos.y, pos.z);

	//Set terrain_sector to layer
	auto scene = CManagement::GetInstance()->Get_Scene();
	auto maplayer = scene->Get_Layer();
	auto layerIter = maplayer.find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	layer->Add_GameObject(OBJID::OBJ_TERRAIN, terrainSector);

	return terrainSector;
}

CGameObject* CObjectFactory::Mod_Terrain(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos)
{
	//Set terrain_sector to layer
	auto layerIter = CManagement::GetInstance()->Get_Scene()->Get_Layer().find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	auto pObj = layer->Get_ObjectGroup(OBJID::OBJ_TERRAIN).back();

	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texTag);

	auto terrainSector = dynamic_cast<CTerrainSector*>(pObj);
	terrainSector->Mod_BufferCom(dwCntX, dwCntZ, dwVtxItv, vertices);
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));

	return terrainSector;
}

void CObjectFactory::Free()
{
	m_pGraphicDev->Release();
}
