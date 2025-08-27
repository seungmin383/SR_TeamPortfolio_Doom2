#include "pch.h"
#include "CTextureManager.h"
#include "CPrototypeManager.h"

IMPLEMENT_SINGLETON(CTextureManager)

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
	Free();
}

void CTextureManager::BulkLoadTexture(LPDIRECT3DDEVICE9 pGraphicDev, string str)
{
	vector<fs::path> list = CFileManager::GetInstance()->GetListCurrentDir();

	if (list.empty())
		return;

	_uint repoIdx = (_uint)StringConversion(str);

	auto pm = CPrototypeManager::GetInstance();
	for (auto item : list)
	{
		if (item.extension().string() != ".png" && item.extension().string() != ".PNG")
			continue;

		auto fileName = item.filename().wstring();
		auto filePath = item.wstring();
		pm->Ready_ComPrototype(fileName, CTexture::Create(pGraphicDev, filePath.c_str(), TEX_NORMAL));

		//전체 경로 key로 저장
		textureRepo[repoIdx].insert({ item, dynamic_cast<CTexture*>(pm->Clone_ComPrototype(fileName))});
	}
}

map<fs::path, CTexture*> CTextureManager::GetTextureGroup(string str)
{
	return textureRepo[(_uint)StringConversion(str)];
}

CTexture* CTextureManager::GetTexture(string str, fs::path tag)
{
	auto repo = textureRepo[(_uint)StringConversion(str)];

	auto result = repo.find(tag);

	if (result == repo.end())
		return nullptr;

	return (*result).second;
}

CTextureManager::terrainDir CTextureManager::StringConversion(string str)
{
	if (str == "Ceil")
		return CTextureManager::terrainDir::CEIL;
	else if (str == "Deco")
		return CTextureManager::terrainDir::DECO;
	else if (str == "Door")
		return CTextureManager::terrainDir::DOOR;
	else if (str == "Env")
		return CTextureManager::terrainDir::ENV;
	else if (str == "Flat")
		return CTextureManager::terrainDir::FLAT;
	else if (str == "Floor")
		return CTextureManager::terrainDir::FLOOR;
	else if (str == "LightTile")
		return CTextureManager::terrainDir::LIGHTTILE;
	else if (str == "Obstacles")
		return CTextureManager::terrainDir::OBSTACLES;
	else if (str == "Step")
		return CTextureManager::terrainDir::STEP;
	else if (str == "Switch")
		return CTextureManager::terrainDir::SWITCH;
	else if (str == "Wall")
		return CTextureManager::terrainDir::WALL;
	else
		return CTextureManager::terrainDir::ETC;
}

void CTextureManager::Free()
{
	for (auto m : textureRepo)
		for_each(m.begin(), m.end(), CDeleteMap());
}
