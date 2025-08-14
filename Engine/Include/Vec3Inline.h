#pragma once
#include "Engine_Define.h"

inline void to_json(json& j, const _vec3& v3)
{
	j["x"] = v3.x;
	j["y"] = v3.y;
	j["z"] = v3.z;
}

inline void from_json(const json& j, _vec3& v3)
{
	j.at("x").get_to(v3.x);
	j.at("y").get_to(v3.y);
	j.at("z").get_to(v3.z);
}