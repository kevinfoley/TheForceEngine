#pragma once
#include <TFE_System/types.h>
#include <TFE_Level/rsector.h>
#include <TFE_Level/rwall.h>
#include <TFE_Level/robject.h>
#include "infMessageType.h"

struct TextureData;
struct Allocator;

using namespace TFE_Level;

namespace TFE_InfSystem
{
	typedef void(*InfLinkMsgFunc)(InfMessageType);
	typedef void(*InfFreeFunc)(void*);
	struct InfLink;

	// Core elevator types.
	enum InfElevatorType
	{
		IELEV_MOVE_CEILING = 0,
		IELEV_MOVE_FLOOR = 1,
		IELEV_MOVE_OFFSET = 2,
		IELEV_MOVE_WALL = 3,
		IELEV_ROTATE_WALL = 4,
		IELEV_SCROLL_WALL = 5,
		IELEV_SCROLL_FLOOR = 6,
		IELEV_SCROLL_CEILING = 7,
		IELEV_CHANGE_LIGHT = 8,
		IELEV_MOVE_FC = 9,
		IELEV_CHANGE_WALL_LIGHT = 10,
	};

	// "Special" elevators are "high level" elevators that map to the core
	// 11 types (see InfElevatorType) but have special settings and/or
	// automatically add stops to make commonly used patterns easier to setup.
	enum InfSpecialElevator
	{
		IELEV_SP_BASIC = 0,
		IELEV_SP_BASIC_AUTO,
		// Both of these are unimplemented in the final game.
		IELEV_SP_UNIMPLEMENTED,
		IELEV_SP_MID,
		// Back to implemented types.
		IELEV_SP_INV = 4,
		IELEV_SP_DOOR,
		IELEV_SP_DOOR_INV,
		IELEV_SP_DOOR_MID,
		IELEV_SP_MORPH_SPIN1,
		IELEV_SP_MORPH_SPIN2,
		IELEV_SP_MORPH_MOVE1,
		IELEV_SP_MORPH_MOVE2,
		IELEV_SP_EXPLOSIVE_WALL,
		IELEV_SP_COUNT
	};

	// How an elevator moves if triggered.
	enum ElevTrigMove
	{
		TRIGMOVE_HOLD = -1,	// The elevator is holding, so the trigger message has no effect.
		TRIGMOVE_CONT = 0,	// Continue to the next stop if NOT already moving.
		TRIGMOVE_LAST = 1,	// Set next (target) stop to the last stop whether moving or not.
		TRIGMOVE_NEXT = 2,	// Set next (target) stop to the next stop if NOT waiting for a timed delay.
		TRIGMOVE_PREV = 3,	// Set next (target) stop to the prev stop if waiting for a timed delay or goes back 2 stops if NOT waiting for a timed delay.
	};

	enum ElevUpdateFlags
	{
		ELEV_MOVING = (1 << 0),			// elevator should be moving.
		ELEV_MASTER_ON = (1 << 1),		// master on
		ELEV_MOVING_REVERSE = (1 << 2),	// the elevator is moving in reverse.
	};

	enum InfDelay
	{
		// IDELAY_SECONDS < IDELAY_COMPLETE
		IDELAY_HOLD = 0xffffffff,
		IDELAY_TERMINATE = 0xfffffffe,
		IDELAY_COMPLETE = 0xfffffffd,
	};

	enum LinkType
	{
		LTYPE_SECTOR = 0,
		LTYPE_TRIGGER = 1,
		LTYPE_TELEPORT = 2,
	};

	enum TriggerType
	{
		ITRIGGER_WALL = 0,
		ITRIGGER_SECTOR = 1,
		ITRIGGER_SWITCH1 = 2,
		ITRIGGER_TOGGLE = 3,
		ITRIGGER_SINGLE = 4,
	};

	enum InfEventMask
	{
		INF_EVENT_CROSS_LINE_FRONT = (1u << 0u),
		INF_EVENT_CROSS_LINE_BACK = (1u << 1u),
		INF_EVENT_ENTER_SECTOR = (1u << 2u),
		INF_EVENT_LEAVE_SECTOR = (1u << 3u),
		INF_EVENT_NUDGE_FRONT = (1u << 4u),	// front of line or inside sector.
		INF_EVENT_NUDGE_BACK = (1u << 5u),	// back of line or outside sector.
		INF_EVENT_EXPLOSION = (1u << 6u),
		INF_EVENT_UNKNOWN = (1u << 7u),		// skipped slot or unused event?
		INF_EVENT_SHOOT_LINE = (1u << 8u),	// Shoot or punch line.
		INF_EVENT_LAND = (1u << 9u),		// Land on floor
		INF_EVENT_10 = (1u << 10u),			// Unknown event
		INF_EVENT_11 = (1u << 11u),			// Unknown Event
		INF_EVENT_31 = (1u << 31u),			// Unknown Event
		INF_EVENT_ANY = 0xffffffff
	};

	enum InfEntityMask
	{
		INF_ENTITY_ENEMY = (1 << 0),
		INF_ENTITY_WEAPON = (1 << 3),
		INF_ENTITY_11     = (1 << 11),
		INF_ENTITY_PLAYER = (1 << 31),
		INF_ENTITY_ANY = 0xffffffffu
	};

	enum InfElevatorFlags
	{
		INF_EFLAG_MOVE_FLOOR = (1 << 0),	// Move on floor.
		INF_EFLAG_MOVE_SECHT = (1 << 1),	// Move on second height.
		INF_EFLAG_MOVE_CEIL  = (1 << 2),	// Move on ceiling (head has to be close enough?)
		INF_EFLAG_DOOR       = (1 << 3),	// This is a door auto-created from the sector flag.
	};

	enum KeyItem
	{
		KEY_RED = 23,
		KEY_YELLOW = 24,
		KEY_BLUE = 25,
	};

	enum TeleportType
	{
		TELEPORT_BASIC = 0,
		TELEPORT_CHUTE = 1
	};
		
	struct AnimatedTexture
	{
		s32 count;
		s32 index;
		u32 delay;
		s32 nextTick;
		TextureData** frameList;
		TextureData** basePtr;
		TextureData* curFrame;
		u8* baseData;
	};

	struct Teleport
	{
		RSector* sector;
		RSector* target;
		TeleportType type;

		vec3_fixed dstPosition;
		angle14_16  dstAngle[3];
	};

	struct TriggerTarget
	{
		RSector* sector;
		RWall*   wall;
		u32      eventMask;
	};

	struct InfTrigger
	{
		TriggerType type;
		InfLink* link;
		AnimatedTexture* animTex;
		Allocator* targets;
		InfMessageType cmd;
		u32 event;
		s32 arg0;
		s32 arg1;

		s32 u20;
		s32 u24;
		s32 u28;
		s32 u2c;

		u32 u30;
		s32 u34;
		s32 master;
		TextureData* tex;
		s32 soundId;
		s32 state;
		s32* u48;
		u32 textId;
	};

	struct InfMessage
	{
		RSector* sector;
		RWall* wall;
		InfMessageType msgType;
		u32 event;
		s32 arg1;
		s32 arg2;
	};

	struct Slave
	{
		RSector* sector;
		s32 value;
	};

	struct AdjoinCmd
	{
		RWall* wall0;
		RWall* wall1;
		RSector* sector0;
		RSector* sector1;
	};

	struct Stop
	{
		fixed16_16 value;
		u32 delay;				// delay in 'ticks' (145 ticks / second)
		Allocator* messages;
		Allocator* adjoinCmds;
		s32 pageId;
		TextureData* floorTex;
		TextureData* ceilTex;
	};

	struct InfElevator
	{
		InfElevator* self;
		InfElevatorType type;
		ElevTrigMove trigMove;
		RSector* sector;
		u32 key;
		s32 fixedStep;
		u32 nextTick;
		s32 u1c;
		Allocator* stops;
		Allocator* slaves;
		Stop* nextStop;
		fixed16_16 speed;
		fixed16_16* value;
		fixed16_16 iValue;
		vec2_fixed dirOrCenter;
		u32 flags;
		s32 sound0;
		s32 sound1;
		s32 sound2;
		s32 soundSource1;
		s32 u54;
		s32 updateFlags;
	};

	struct InfLink
	{
		LinkType type;				// Sector or Trigger
		InfLinkMsgFunc msgFunc;		// Either the Elevator or Trigger msg func.
		union
		{
			InfElevator* elev;		// The actual INF item.
			InfTrigger* trigger;
			void* target;
		};
		u32 eventMask;				// The event mask which helps determine if a link can be activated.
		u32 entityMask;				// The entity mask (as above).
		Allocator* parent;			// The parent list of links.
		InfFreeFunc freeFunc;		// The function to use to free the link.
	};
}