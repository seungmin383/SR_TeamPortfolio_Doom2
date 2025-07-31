#pragma once

enum class LAYERID
{
	LAYER_ENVIRONMENT,
	LAYER_GAMELOGIC,
	LAYER_UI,
	LAYER_CAMERA,
	LAYER_SKYBOX,
	LAYER_END
};

/// <summary>
/// 오브젝트 분류
/// </summary>
enum class OBJID
{
	OBJ_PLAYER,
	OBJ_ENEMTY,

	OBJ_OBJECT,
	OBJ_TERRAIN,
	OBJ_WALL,
	
	OBJ_ATTACK,
	OBJ_BULLET,
	OBJ_LAY,
	
	OBJ_ITEM,
	OBJ_BUTTON,
	
	OBJ_EXPLOSION,
	OBJ_FOG,

	OBJ_UI,

	OBJ_DYNAMICCAMERA,
	OBJ_PLAYERCAMERA,

	OBJ_WEAPON,

	OBJ_BACKGROUND,
	OBJ_SKYBOX,

	OBJ_END
};

/// <summary>
/// 충돌 분류
/// ex)
/// m_eColLayerId = COL_PLAYER;
/// m_iColMask = COL_ITEM | COL_OBJECT | COL_TERRAIN | COL_WALL | COL_ATTACK;
///
/// 충돌 확인
/// if ( (PlayerCollider->m_iColMask & ItemCollider->m_eColLayerId) != 0)
///		//처리 코드
/// </summary>
enum COLLAYERID
{
	COL_PLAYER = 1 << 0,
	COL_ENEMY = 1 << 1,
	
	COL_OBJECT = 1 << 2,
	COL_TERRAIN = 1 << 3,
	COL_WALL = 1 << 4,

	COL_ATTACK = 1 << 5,
	COL_BULLET = 1 << 6,
	COL_LAY = 1 << 7,

	COL_ITEM = 1 << 8,
	COL_BUTTON = 1 << 9,

	COL_EXPLOSION = 1 << 10,
	COL_FOG = 1 << 11,

	COL_UI = 1 << 12,

	COL_DYNAMICCAMERA = 1 << 13,
	COL_PLAYERCAMERA = 1 << 14,

	COL_WEAPON = 1 << 15,

	COL_BACKGROUND = 1 << 16,
	COL_SKYBOX = 1 << 17,

	COL_END
};

/// <summary>
/// 사운드 채널
/// </summary>
enum CHANNELID
{
	SOUND_EFFECT,
	SOUND_BGM,
	SOUND_PLAY,
	MAXCHANNEL
};