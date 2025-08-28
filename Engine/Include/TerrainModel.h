#pragma once
#include "Engine_Define.h"
#include "Vec3Inline.h"

struct TerrainModel
{
	int idx;
	int type;
	int vertexCountX;
	int vertexCountZ;
	float vertexIntervalX;
	float vertexIntervalY;
	float vertexIntervalZ;

	_vec3 pos;
	vector<_vec3> vertices;
	string strTextureTag;
};

inline void to_json(json& j, const TerrainModel& t)
{
	j["idx"] = t.idx;
	j["type"] = t.type;
	j["vertexCountX"] = t.vertexCountX;
	j["vertexCountZ"] = t.vertexCountZ;
	j["vertexIntervalX"] = t.vertexIntervalX;
	j["vertexIntervalY"] = t.vertexIntervalY;
	j["vertexIntervalZ"] = t.vertexIntervalZ;

	j["pos"] = t.pos;
	j["vertices"] = t.vertices;
	j["strTextureTag"] = t.strTextureTag;
}

inline void from_json(const json& j, TerrainModel& t)
{
	j.at("idx").get_to(t.idx);
	j.at("type").get_to(t.type);
	j.at("vertexCountX").get_to(t.vertexCountX);
	j.at("vertexCountZ").get_to(t.vertexCountZ);
	j.at("vertexIntervalX").get_to(t.vertexIntervalX);
	j.at("vertexIntervalY").get_to(t.vertexIntervalY);
	j.at("vertexIntervalZ").get_to(t.vertexIntervalZ);

	j.at("pos").get_to(t.pos);
	j.at("vertices").get_to(t.vertices);
	j.at("strTextureTag").get_to(t.strTextureTag);
}