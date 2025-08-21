#pragma once
#include "Engine_Define.h"

struct GameObjectType
{
	_uint value;
	string tag;
};

inline void to_json(json& j, const GameObjectType& c)
{
	j["value"] = c.value;
	j["tag"] = c.tag;
}

inline void from_json(const json& j, GameObjectType& c)
{
	j.at("value").get_to(c.value);
	j.at("tag").get_to(c.tag);
}