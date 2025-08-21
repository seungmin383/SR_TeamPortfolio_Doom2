#pragma once
#include "Engine_Define.h"

struct StageSet
{
	_int StageNumber;
	string terrainFileName;
	string ObjectFileName;
	string MonsterFileName;
};

inline void to_json(json& j, const StageSet& c)
{
	j["StageNumber"] = c.StageNumber;
	j["terrainFileName"] = c.terrainFileName;
	j["ObjectFileName"] = c.ObjectFileName;
	j["MonsterFileName"] = c.MonsterFileName;
}

inline void from_json(const json& j, StageSet& c)
{
	j.at("StageNumber").get_to(c.StageNumber);
	j.at("terrainFileName").get_to(c.terrainFileName);
	j.at("ObjectFileName").get_to(c.ObjectFileName);
	j.at("MonsterFileName").get_to(c.MonsterFileName);
}