#pragma once
//////////////////////////////////////////////////////////////////////
// The Force Engine Keyword Table.
// This table is used when parsing Dark Forces data files. The list of
// strings was extracted from the DF executable.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include <string>
#include <vector>

// Keywords used by Dark Forces.
// Note: there are some repeats, so there are a few elements called KW_xxx2; for example KW_KEY2
enum KEYWORD
{
	KW_UNKNOWN = -1,
	KW_VISIBLE = 0,
	KW_SHADED,
	KW_LIGHT,
	KW_PARENT,
	KW_D_X,
	KW_D_Y,
	KW_D_Z,
	KW_D_PITCH,
	KW_D_YAW,
	KW_D_ROLL,
	KW_D_VIEW_PITCH,
	KW_D_VIEW_YAW,
	KW_D_VIEW_ROLL,
	KW_VIEW_PITCH,
	KW_VIEW_YAW,
	KW_VIEW_ROLL,
	KW_TYPE,
	KW_LOGIC,
	KW_VUE,
	KW_VUE_APPEND,
	KW_EYE,
	KW_BOSS,
	KW_UPDATE,
	KW_EYE_D_XYZ,
	KW_EYE_D_PYR,
	KW_SYNC,
	KW_FRAME_RATE,
	KW_START,
	KW_STOP_Y,
	KW_STOP,
	KW_SPEED,
	KW_MASTER,
	KW_ANGLE,
	KW_PAUSE,
	KW_ADJOIN,
	KW_TEXTURE,
	KW_SLAVE,
	KW_TRIGGER_ACTION,
	KW_CONDITION,
	KW_CLIENT,
	KW_MESSAGE,
	KW_TEXT,
	KW_EVENT,
	KW_EVENT_MASK,
	KW_ENTITY_MASK,
	KW_OBJECT_MASK,
	KW_CENTER,
	KW_KEY,
	KW_ADDON,
	KW_FLAGS,
	KW_SOUND_COLON,
	KW_PAGE,
	KW_SOUND,
	KW_SYSTEM,
	KW_SAFE,
	KW_LEVEL,
	KW_AMB_SOUND,
	KW_ELEVATOR,
	KW_BASIC,
	KW_BASIC_AUTO,
	KW_ENCLOSED,
	KW_INV,
	KW_MID,
	KW_DOOR,
	KW_DOOR_INV,
	KW_DOOR_MID,
	KW_MORPH_SPIN1,
	KW_MORPH_SPIN2,
	KW_MORPH_MOVE1,
	KW_MORPH_MOVE2,
	KW_MOVE_CEILING,
	KW_MOVE_FLOOR,
	KW_MOVE_FC,
	KW_MOVE_OFFSET,
	KW_MOVE_WALL,
	KW_ROTATE_WALL,
	KW_SCROLL_WALL,
	KW_SCROLL_FLOOR,
	KW_SCROLL_CEILING,
	KW_CHANGE_LIGHT,
	KW_CHANGE_WALL_LIGHT,
	KW_TRIGGER,			// This is the trigger ID that will be found.
	KW_SWITCH1,
	KW_TOGGLE,
	KW_SINGLE,
	KW_STANDARD,
	KW_TELEPORTER,
	KW_ENTITY_ENTER,
	KW_SECTOR,
	KW_LINE,
	KW_TARGET,
	KW_MOVE,
	KW_CHUTE,
	KW_TRIGGER2,		// This is a repeat and should never be found directly.
	KW_NEXT_STOP,
	KW_PREV_STOP,
	KW_GOTO_STOP,
	KW_MASTER_ON,
	KW_MASTER_OFF,
	KW_DONE,
	KW_SET_BITS,
	KW_CLEAR_BITS,
	KW_COMPLETE,
	KW_LIGHTS,
	KW_WAKEUP,
	KW_NONE,
	KW_STORM,
	KW_ENTITY,
	KW_PLAYER,
	KW_TROOP,
	KW_STORM1,
	KW_INT_DROID,
	KW_PROBE_DROID,
	KW_D_TROOP1,
	KW_D_TROOP2,
	KW_D_TROOP3,
	KW_BOBA_FETT,
	KW_CAMMANDO,
	KW_I_OFFICER,
	KW_I_OFFICER1,
	KW_I_OFFICER2,
	KW_I_OFFICER3,
	KW_I_OFFICER4,
	KW_I_OFFICER5,
	KW_I_OFFICER6,
	KW_I_OFFICER7,
	KW_I_OFFICER8,
	KW_I_OFFICER9,
	KW_I_OFFICERR,
	KW_I_OFFICERY,
	KW_I_OFFICERB,
	KW_G_GUARD,
	KW_REE_YEES,
	KW_REE_YEES2,
	KW_BOSSK,
	KW_BARREL,
	KW_LAND_MINE,
	KW_KELL,
	KW_SEWER1,
	KW_REMOTE,
	KW_TURRET,
	KW_MOUSEBOT,
	KW_WELDER,
	KW_SCENERY,
	KW_ANIM,
	KW_KEY_COLON,
	KW_GENERATOR,
	KW_TRUE,
	KW_FALSE,
	KW_3D,
	KW_SPRITE,
	KW_FRAME,
	KW_SPIRIT,
	KW_ITEM,
	KW_DISPATCH,
	KW_RADIUS,
	KW_HEIGHT,
	KW_BATTERY,
	KW_BLUE,
	KW_CANNON,
	KW_CLEATS,
	KW_CODE1,
	KW_CODE2,
	KW_CODE3,
	KW_CODE4,
	KW_CODE5,
	KW_CODE6,
	KW_CODE7,
	KW_CODE8,
	KW_CODE9,
	KW_CONCUSSION,
	KW_DETONATOR,
	KW_DETONATORS,
	KW_DT_WEAPON,
	KW_DATATAPE,
	KW_ENERGY,
	KW_FUSION,
	KW_GOGGLES,
	KW_MASK,
	KW_MINE,
	KW_MINES,
	KW_MISSLE,
	KW_MISSLES,
	KW_MORTAR,
	KW_NAVA,
	KW_PHRIK,
	KW_PLANS,
	KW_PLASMA,
	KW_POWER,
	KW_RED,
	KW_RIFLE,
	KW_SHELL,
	KW_SHELLS,
	KW_SHIELD,
	KW_INVINCIBLE,
	KW_REVIVE,
	KW_SUPERCHARGE,
	KW_LIFE,
	KW_MEDKIT,
	KW_PILE,
	KW_YELLOW,
	KW_AUTOGUN,
	KW_DELAY,
	KW_INTERVAL,
	KW_MAX_ALIVE,
	KW_MIN_DIST,
	KW_MAX_DIST,
	KW_NUM_TERMINATE,
	KW_WANDER_TIME,
	KW_PLUGIN,
	KW_THINKER,
	KW_FOLLOW,
	KW_FOLLOW_Y,
	KW_RANDOM_YAW,
	KW_MOVER,
	KW_SHAKER,
	KW_PERSONALITY,
	KW_SEQEND,
	KW_COUNT
};

extern KEYWORD getKeywordIndex(const char* keywordString);