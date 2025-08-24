#pragma once
#include "Engine_Define.h"
#include "TerrainModelMod.h"

struct TerrainModelsMod
{
	vector<TerrainModelMod> model;
};

inline void to_json(json& j, const TerrainModelsMod& t)
{
	j["model"] = t.model;
}

inline void from_json(const json& j, TerrainModelsMod& t)
{
	j.at("model").get_to(t.model);
}