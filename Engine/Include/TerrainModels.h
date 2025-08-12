#pragma once
#include "Engine_Define.h"
#include "TerrainModel.h"

struct TerrainModels
{
	vector<TerrainModel> model;
};

inline void to_json(json& j, const TerrainModels& t)
{
	j["model"] = t.model;
}

inline void from_json(const json& j, TerrainModels& t)
{
	j.at("model").get_to(t.model);
}