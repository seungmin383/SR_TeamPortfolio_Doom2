#pragma once
#include "Engine_Define.h"
#include "GameObjectSpawnInfo.h"

struct SceneSpawnInfo
{
	vector<GameObjectSpawnInfo> objects;
	vector<GameObjectSpawnInfo> monsters;
};

inline void to_json(json& j, const SceneSpawnInfo& s)
{
	j["objects"] = s.objects;
	j["monsters"] = s.monsters;
}

inline void from_json(const json& j, SceneSpawnInfo& s)
{
	j.at("objects").get_to(s.objects);
	j.at("monsters").get_to(s.monsters);
}