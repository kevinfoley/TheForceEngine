#pragma once
//////////////////////////////////////////////////////////////////////
// Dark Forces Cheats
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>

namespace TFE_DarkForces
{
	enum CheatID
	{
		CHEAT_NONE = 0,
		CHEAT_LACDS,
		CHEAT_LANTFH,
		CHEAT_LAPOGO,
		CHEAT_LARANDY,
		CHEAT_LAIMLAME,
		CHEAT_LAPOSTAL,
		CHEAT_LADATA,
		CHEAT_LABUG,
		CHEAT_LAREDLITE,
		CHEAT_LASECBASE,
		CHEAT_LATALAY,
		CHEAT_LASEWERS,
		CHEAT_LATESTBASE,
		CHEAT_LAGROMAS,
		CHEAT_LADTENTION,
		CHEAT_LARAMSHED,
		CHEAT_LAROBOTICS,
		CHEAT_LANARSHADA,
		CHEAT_LAJABSHIP,
		CHEAT_LAIMPCITY,
		CHEAT_LAFUELSTAT,
		CHEAT_LAEXECUTOR,
		CHEAT_LAARC,
		CHEAT_LASKIP,
		CHEAT_LABRADY,
		CHEAT_LAUNLOCK,
		CHEAT_LAMAXOUT,
		// New Cheats for TFE.
		CHEAT_LAFLY,
		CHEAT_LANOCLIP,
		CHEAT_LATESTER,
		CHEAT_LAADDLIFE,
		CHEAT_LASUBLIFE,
		CHEAT_LACAT,
		CHEAT_LADIE,
		CHEAT_LAIMDEATH,
		CHEAT_LAHARDCORE,
		CHEAT_LABRIGHT,
		CHEAT_COUNT
	};

	CheatID cheat_getID();
	CheatID cheat_getIDFromString(const char* cheatStr);
	const char* cheat_getStringFromID(CheatID id);
	void cheat_clearData();

	extern char s_cheatString[32];
	extern s32  s_cheatCharIndex;
	extern s32  s_cheatInputCount;
}  // namespace TFE_DarkForces