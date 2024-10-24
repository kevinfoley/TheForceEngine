#include "UIS_FrontEndUi.h"
#include <TFE_System/system.h>
#include <TFE_ForceScript/float2.h>
#include <TFE_FrontEndUI/frontEndUi.h>
#include <TFE_RenderBackend/renderBackend.h>
#include <TFE_Ui/ui.h>


#ifdef ENABLE_FORCE_SCRIPT
#include <angelscript.h>
#include "uis_darkForces.h"
#include <TFE_DarkForces/player.h>
#include <TFE_DarkForces/hud.h>
#include <TFE_DarkForces/mission.h>
#include <TFE_Jedi/Renderer/rcommon.h>
namespace TFE_FrontEndUI
{

	////////////////////////////////////////////////////////
	//  Functions
	////////////////////////////////////////////////////////
	// Cheats.
	//   - LAIMLAME
	bool UIS_DarkForces::cheat_getInvincible()
	{
		return TFE_DarkForces::s_invincibility;
	}
	void UIS_DarkForces::cheat_toggleInvincible()
	{
		TFE_DarkForces::cheat_godMode();
	}

	//   - LAREDLITE
	bool UIS_DarkForces::cheat_getAiDisabled()
	{
		return ~TFE_DarkForces::s_aiActive;
	}
	void UIS_DarkForces::cheat_toggleAi()
	{
		TFE_DarkForces::cheat_pauseAI();
	}

	//   - LAPOGO
	bool UIS_DarkForces::cheat_getHeightCheckDisabled()
	{
		return ~TFE_DarkForces::s_limitStepHeight;
	}
	void UIS_DarkForces::cheat_toggleHeightCheck()
	{
		TFE_DarkForces::cheat_toggleHeightCheck();
	}

	//   - LABUG
	bool UIS_DarkForces::cheat_getSmallMode()
	{
		return TFE_DarkForces::s_smallModeEnabled;
	}
	void UIS_DarkForces::cheat_toggleSmallMode()
	{
		TFE_DarkForces::cheat_bugMode();
	}

	//   - LADATA
	bool UIS_DarkForces::cheat_getData()
	{
		return TFE_DarkForces::s_showData;
	}
	void UIS_DarkForces::cheat_toggleData()
	{
		TFE_DarkForces::cheat_toggleData();
	}

	//   - LAFLY
	bool UIS_DarkForces::cheat_getFly()
	{
		return TFE_DarkForces::s_flyMode;
	}
	void UIS_DarkForces::cheat_toggleFly()
	{
		TFE_DarkForces::cheat_fly();
	}

	//   - LABRIGHT
	bool UIS_DarkForces::cheat_getFullBright()
	{
		return TFE_Jedi::s_fullBright;
	}
	void UIS_DarkForces::cheat_toggleFullBright()
	{
		TFE_DarkForces::cheat_toggleFullBright();
	}

	// LANOCLIP
	bool UIS_DarkForces::cheat_getNoClip()
	{
		return TFE_DarkForces::s_noclip;
	}
	void UIS_DarkForces::cheat_toggleNoClip()
	{
		TFE_DarkForces::cheat_noclip();
	}


	////////////////////////////////////////////////////////
	//  System
	////////////////////////////////////////////////////////
	bool UIS_DarkForces::scriptRegister(asIScriptEngine* engine)
	{
		s32 res = 0;

		////////////////////////////////////////////////////////
		//  OBJECTS 
		////////////////////////////////////////////////////////

		// This helper class:
		res = engine->RegisterObjectType("DarkForces", sizeof(UIS_DarkForces), asOBJ_VALUE | asOBJ_POD); assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getInvincible()", asMETHOD(UIS_DarkForces, cheat_getInvincible), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleInvincible()", asMETHOD(UIS_DarkForces, cheat_toggleInvincible), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getAiDisabled()", asMETHOD(UIS_DarkForces, cheat_getAiDisabled), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleAi()", asMETHOD(UIS_DarkForces, cheat_toggleAi), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getHeightCheckDisabled()", asMETHOD(UIS_DarkForces, cheat_getHeightCheckDisabled), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleHeightCheck()", asMETHOD(UIS_DarkForces, cheat_toggleHeightCheck), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getSmallMode()", asMETHOD(UIS_DarkForces, cheat_getSmallMode), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleSmallMode()", asMETHOD(UIS_DarkForces, cheat_toggleSmallMode), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getData()", asMETHOD(UIS_DarkForces, cheat_getData), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleData()", asMETHOD(UIS_DarkForces, cheat_toggleData), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getFly()", asMETHOD(UIS_DarkForces, cheat_getFly), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleFly()", asMETHOD(UIS_DarkForces, cheat_toggleFly), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getFullBright()", asMETHOD(UIS_DarkForces, cheat_getFullBright), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleFullBright()", asMETHOD(UIS_DarkForces, cheat_toggleFullBright), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "bool cheat_getNoClip()", asMETHOD(UIS_DarkForces, cheat_getNoClip), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("DarkForces", "void cheat_toggleNoClip()", asMETHOD(UIS_DarkForces, cheat_toggleNoClip), asCALL_THISCALL);  assert(res >= 0);


		res = engine->RegisterGlobalProperty("DarkForces darkForces", this);  assert(res >= 0);

		////////////////////////////////////////////////////////
		//  Globals
		//      Alternative to wrapper methods.
		//      Much less boilerplate, but I hate globals...
		////////////////////////////////////////////////////////
		//   - LAIMDEATH
		res = engine->RegisterGlobalProperty("bool cheat_oneHitKill", &TFE_DarkForces::s_oneHitKillEnabled); assert(res >= 0);
		res = engine->RegisterGlobalFunction("void cheat_toggleOneHitKill()", asFUNCTION(TFE_DarkForces::cheat_oneHitKill), asCALL_CDECL); assert(res >= 0);
		//   - LAHARDCORE
		res = engine->RegisterGlobalProperty("bool cheat_instantDeath", &TFE_DarkForces::s_instaDeathEnabled); assert(res >= 0);
		res = engine->RegisterGlobalFunction("void cheat_toggleInstantDeath()", asFUNCTION(TFE_DarkForces::cheat_instaDeath), asCALL_CDECL); assert(res >= 0);
		//   - Non-boolean cheats
		res = engine->RegisterGlobalFunction("void cheat_supercharge()", asFUNCTION(TFE_DarkForces::cheat_supercharge), asCALL_CDECL); assert(res >= 0);
		res = engine->RegisterGlobalFunction("void cheat_revealMap()", asFUNCTION(TFE_DarkForces::cheat_revealMap), asCALL_CDECL); assert(res >= 0);
		res = engine->RegisterGlobalFunction("void cheat_postal()", asFUNCTION(TFE_DarkForces::cheat_postal), asCALL_CDECL); assert(res >= 0);
		return res >= 0;
	}
}
#endif