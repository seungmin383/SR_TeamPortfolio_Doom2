#pragma once
#include "Engine_Define.h"
#include "Vec3Inline.h"

struct TerrainModelMod
{
	int idx;
	bool blend;
	int type = 0;
	int vertexCountX;
	int vertexCountZ;
	float vertexIntervalX;
	float vertexIntervalZ;
	float vertexIntervalY;

	_vec3 pos;
	vector<_vec3> vertices;
	string strTextureTag;
};

inline void to_json(json& j, const TerrainModelMod& t)
{
	j["idx"] = t.idx;
	j["blend"] = t.blend;
	j["type"] = t.type;
	j["vertexCountX"] = t.vertexCountX;
	j["vertexCountZ"] = t.vertexCountZ;
	j["vertexIntervalX"] = t.vertexIntervalX;
	j["vertexIntervalZ"] = t.vertexIntervalZ;
	j["vertexIntervalY"] = t.vertexIntervalY;

	j["pos"] = t.pos;
	j["vertices"] = t.vertices;
	j["strTextureTag"] = t.strTextureTag;
}

inline void from_json(const json& j, TerrainModelMod& t)
{
	j.at("idx").get_to(t.idx);
	j.at("blend").get_to(t.blend);
	j.at("type").get_to(t.type);
	j.at("vertexCountX").get_to(t.vertexCountX);
	j.at("vertexCountZ").get_to(t.vertexCountZ);
	j.at("vertexIntervalX").get_to(t.vertexIntervalX);
	j.at("vertexIntervalZ").get_to(t.vertexIntervalZ);
	j.at("vertexIntervalY").get_to(t.vertexIntervalY);

	j.at("pos").get_to(t.pos);
	j.at("vertices").get_to(t.vertices);
	j.at("strTextureTag").get_to(t.strTextureTag);
}