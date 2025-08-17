#pragma once
#include "Engine_Define.h"
#include "TerrainModels.h"
#include "SceneSpawnInfo.h"

struct StageInfo
{
	//지형
	TerrainModels terrain;

	//아이템, 몬스터
	//spawnInfo.objects
	//spawnInfo.monsters
	SceneSpawnInfo spawnInfo;
};