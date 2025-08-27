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

void CObjectFactory::Create_Object(OBJPROTOTYPE eprototype, OBJID eObjId, _vec3 vPos)
{
	//auto object = CPrototypeManager::GetInstance()->GetInstance()->Clone_ObjPrototype(eprototype);
	CManagement::GetInstance()->Set_GameLogicObject(eprototype, eObjId, vPos);
}

CGameObject* CObjectFactory::Create_Terrain_Default(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, _tchar* texTag, _vec3 pos, OBJID eObjId)
{
	auto sectorBuffer = Engine::CTerrainTexSector::Create_Default(m_pGraphicDev, dwCntX, dwCntZ, dwVtxItv);
	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texTag);

	auto terrainSector = dynamic_cast<CTerrainSector*>(CPrototypeManager::GetInstance()->GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::TERRAINSECTOR));

	terrainSector->Set_BufferCom(sectorBuffer);
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));
	terrainSector->Set_Index(0);

	terrainSector->Get_Transform()->Set_Pos(pos.x, pos.y, pos.z);

	//Set terrain_sector to layer
	auto scene = CManagement::GetInstance()->Get_Scene();
	auto maplayer = scene->Get_Layer();
	auto layerIter = maplayer.find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	layer->Add_GameObject(eObjId, terrainSector);

	return terrainSector;
}

CGameObject* CObjectFactory::Create_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos, OBJID eObjId)
{
	auto sectorBuffer = Engine::CTerrainTexSector::Create(m_pGraphicDev, vertices, dwCntX, dwCntZ, dwVtxItv);
	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texTag);

	auto terrainSector = dynamic_cast<CTerrainSector*>(CPrototypeManager::GetInstance()->GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::TERRAINSECTOR));

	terrainSector->Set_BufferCom(sectorBuffer);
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));
	terrainSector->Set_Index(idx);

	terrainSector->Get_Transform()->Set_Pos(pos.x, pos.y, pos.z);

	//Set terrain_sector to layer
	auto scene = CManagement::GetInstance()->Get_Scene();
	auto maplayer = scene->Get_Layer();
	auto layerIter = maplayer.find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	layer->Add_GameObject(eObjId, terrainSector);

	return terrainSector;
}

CGameObject* CObjectFactory::Mod_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, _tchar* texTag, _vec3 pos, OBJID eObjId)
{
	//Set terrain_sector to layer
	auto layerIter = CManagement::GetInstance()->Get_Scene()->Get_Layer().find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	auto pObj = layer->Get_ObjectGroup(eObjId).back();

	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texTag);

	auto terrainSector = dynamic_cast<CTerrainSector*>(pObj);
	terrainSector->Mod_BufferCom(dwCntX, dwCntZ, dwVtxItv, vertices);
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));

	return terrainSector;
}

CGameObject* CObjectFactory::Load_Terrain(_int idx, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, vector<_vec3> vertices, fs::path texPath, _vec3 pos, OBJID eObjId)
{
	CTexture* tempTex = CTexture::Create(m_pGraphicDev, texPath.wstring().c_str(), TEX_NORMAL);
	if (FAILED(CPrototypeManager::GetInstance()->Ready_ComPrototype(texPath.filename().wstring().c_str(), tempTex)))
		Safe_Release(tempTex);

	auto texture = CPrototypeManager::GetInstance()->Clone_ComPrototype(texPath.filename().wstring().c_str());

	auto terrainSector = dynamic_cast<CTerrainSector*>(CPrototypeManager::GetInstance()->GetInstance()->Clone_ObjPrototype(OBJPROTOTYPE::TERRAINSECTOR));

	terrainSector->Set_BufferCom(Engine::CTerrainTexSector::Create(m_pGraphicDev, vertices, dwCntX, dwCntZ, dwVtxItv));
	terrainSector->Set_TextureCom(dynamic_cast<CTexture*>(texture));
	terrainSector->Set_Index(idx);

	terrainSector->Get_Transform()->Set_Pos(pos.x, pos.y, pos.z);

	//Set terrain_sector to layer
	auto scene = CManagement::GetInstance()->Get_Scene();
	auto maplayer = scene->Get_Layer();
	auto layerIter = maplayer.find(LAYERID::LAYER_ENVIRONMENT);
	auto layer = (*layerIter).second;
	layer->Add_GameObject(eObjId, terrainSector);

	return terrainSector;
}

void CObjectFactory::Free()
{
	m_pGraphicDev->Release();
}
