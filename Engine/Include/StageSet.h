#pragma once
#include "Engine_Define.h"

struct StageSet
{
	_int StageNumber;
	string terrainFileName;
	string ObjectFileName;
	string MonsterFileName;
	vector<string> AdditionalObject;
	vector<string> AdditionalMonster;
};

inline void to_json(json& j, const StageSet& c)
{
	j["StageNumber"] = c.StageNumber;
	j["terrainFileName"] = c.terrainFileName;
	j["ObjectFileName"] = c.ObjectFileName;
	j["MonsterFileName"] = c.MonsterFileName;
	j["AdditionalObject"] = c.AdditionalObject;
	j["AdditionalMonster"] = c.AdditionalMonster;
}

inline void from_json(const json& j, StageSet& c)
{
	j.at("StageNumber").get_to(c.StageNumber);
	j.at("terrainFileName").get_to(c.terrainFileName);
	j.at("ObjectFileName").get_to(c.ObjectFileName);
	j.at("MonsterFileName").get_to(c.MonsterFileName);
	j.at("AdditionalObject").get_to(c.AdditionalObject);
	j.at("AdditionalMonster").get_to(c.AdditionalMonster);
}