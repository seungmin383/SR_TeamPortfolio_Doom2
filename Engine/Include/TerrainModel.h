#pragma once
#include "Engine_Define.h"
#include "Vec3Inline.h"

struct TerrainModel
{
	int vertexCountX;
	int vertexCountZ;
	int vertexInterval;

	vector<_vec3> vertices;
	string strTextureTag;
};

inline void to_json(json& j, const TerrainModel& t)
{
	j["vertexCountX"] = t.vertexCountX;
	j["vertexCountZ"] = t.vertexCountZ;
	j["vertexInterval"] = t.vertexInterval;

	j["vertices"] = t.vertices;
	j["strTextureTag"] = t.strTextureTag;
}

inline void from_json(const json& j, TerrainModel& t)
{
	j.at("vertexCountX").get_to(t.vertexCountX);
	j.at("vertexCountZ").get_to(t.vertexCountZ);
	j.at("vertexInterval").get_to(t.vertexInterval);

	j.at("vertices").get_to(t.vertices);
	j.at("strTextureTag").get_to(t.strTextureTag);
}