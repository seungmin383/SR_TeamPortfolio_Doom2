#pragma once
#include "Engine_Define.h"
#include "Vec3Inline.h"

struct GameObjectSpawnInfo
{
	/// <summary>
	/// 원본 타입
	/// </summary>
	int type1 = 0;

	/// <summary>
	/// 오브젝트 타입
	/// </summary>
	int type2 = 0;

	/// <summary>
	/// 기타 타입
	/// </summary>
	int type3 = 0;

	/// <summary>
	/// 기타 태그
	/// </summary>
	string tag;

	_vec3 position;
};

inline void to_json(json& j, const GameObjectSpawnInfo& g)
{
	j["type1"] = g.type1;
	j["type2"] = g.type2;
	j["type3"] = g.type3;
	j["tag"] = g.tag;
	j["position"] = g.position;
}

inline void from_json(const json& j, GameObjectSpawnInfo& g)
{
	j.at("type1").get_to(g.type1);
	j.at("type2").get_to(g.type2);
	j.at("type3").get_to(g.type3);
	j.at("tag").get_to(g.tag);
	j.at("position").get_to(g.position);
}