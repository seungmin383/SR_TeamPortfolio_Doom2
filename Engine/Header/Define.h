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
	OBJ_PLAYER = 0,
	OBJ_ENEMY,

	OBJ_OBJECT,
	OBJ_TERRAIN,
	OBJ_WALL,
	
	OBJ_ATTACK,
	OBJ_BULLET,
	OBJ_RAY,
	
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

enum class OBJPROTOTYPE
{
	//0 플레이어
	PLAYER = 0,

	//1 ~ 50 오브젝트 관련
	TERRAIN = 1,
	TERRAINSECTOR,
	WALL,
	DOOR,
	SHIFT_CUBE,
	ITEM_HELTH,
	ITEM_ARMOR,
	ITEM_CLIP,
	ITEM_SHELL,
	OBJ_BARREL,
	BUTTON_DOOR,

	//51 ~ 60 공격 관련
	ATTACK = 51,
	RAY,
	EXPLOSION,
	FOG,
	SWAMP,
	BULLET,

	//61 ~ 70 UI/UX 관련
	UI = 61,
	DYNAMICCAMERA,
	PLAYERCAMERA,
	WEAPON,

	//71 ~ 80 환경 관련
	BACKGROUND = 71,
	SKYBOX,

	//81 ~ 적 관련
	ENEMY_ZOMBIEMAN = 81,
	ENEMY_SHOTGUNGUY,
	ENEMY_IMP,
	ENEMY_DEMON,
	ENEMY_ARCHVILE,
	ENEMY_SPIDERMASTERMIND,

	OBJPROTOTYPE_END
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
	COL_RAY = 1 << 7,

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

enum class DIRECTION
{
	NORTH,			//{0.f, 0.f 1.f}
	NORTHEAST,		//{1.f, 0.f 1.f}
	EAST,			//{1.f, 0.f 0.f}
	SOUTHEAST,		//{1.f, 0.f -1.f}
	SOUTH,			//{0.f, 0.f -1.f}
	SOUTHWEST,		//{-1.f, 0.f -1.f}
	WEST,			//{-1.f, 0.f 0.f}
	NORTHWEST,		//{-1.f, 0.f 1.f}
	END_DIRECTION
};

enum class STATE
{
	IDLE,
	WALK,
	ATTACK,
	HIT,
	DIE,
	END_STATE
};

enum class SCENEID
{
	LOGO,
	LOADING,
	STAGE1,
	STAGE2,
	STAGE3,
	SCENE_END
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