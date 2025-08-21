#pragma once
#include "Engine_Define.h"
#include "GameObjectType.h"

struct ConfigModel
{
	string ResourcePath = "";
	string PlayerResourcePath = "";
	string MonsterResourcePath = "";
	string UIResourcePath = "";
	string ObjectResourcePath = "";
	string TerrainResourcePath = "";
	
	string ObjectSpawnSavePath = "";
	string MonsterSpawnSavePath = "";
	string TerrainSavePath = "";

	vector<GameObjectType> ObjPrototypeTags;
	vector<GameObjectType> ObjIdTags;
};

inline void to_json(json& j, const ConfigModel& c)
{
	j["ResourcePath"] = c.ResourcePath;
	j["PlayerResourcePath"] = c.PlayerResourcePath;
	j["MonsterResourcePath"] = c.MonsterResourcePath;
	j["UIResourcePath"] = c.UIResourcePath;
	j["ObjectResourcePath"] = c.ObjectResourcePath;
	j["TerrainResourcePath"] = c.TerrainResourcePath;

	j["ObjectSpawnSavePath"] = c.ObjectSpawnSavePath;
	j["MonsterSpawnSavePath"] = c.MonsterSpawnSavePath;
	j["TerrainSavePath"] = c.TerrainSavePath;

	j["ObjPrototypeTags"] = c.ObjPrototypeTags;
	j["ObjIdTags"] = c.ObjIdTags;
}

inline void from_json(const json& j, ConfigModel& c)
{
	j.at("ResourcePath").get_to(c.ResourcePath);
	j.at("PlayerResourcePath").get_to(c.PlayerResourcePath);
	j.at("MonsterResourcePath").get_to(c.MonsterResourcePath);
	j.at("UIResourcePath").get_to(c.UIResourcePath);
	j.at("ObjectResourcePath").get_to(c.ObjectResourcePath);
	j.at("TerrainResourcePath").get_to(c.TerrainResourcePath);

	j.at("ObjectSpawnSavePath").get_to(c.ObjectSpawnSavePath);
	j.at("MonsterSpawnSavePath").get_to(c.MonsterSpawnSavePath);
	j.at("TerrainSavePath").get_to(c.TerrainSavePath);

	j.at("ObjPrototypeTags").get_to(c.ObjPrototypeTags);
	j.at("ObjIdTags").get_to(c.ObjIdTags);
}