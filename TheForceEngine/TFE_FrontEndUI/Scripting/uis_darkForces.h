#pragma once
//////////////////////////////////////////////////////////////////////
// Support for scripting system to render GUI with AngelScript.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/system.h>
#ifdef ENABLE_FORCE_SCRIPT
#include <TFE_System/types.h>
#include <TFE_ForceScript/float2.h>
#include <string>

namespace TFE_FrontEndUI
{
	class UIS_DarkForces
	{
	public:
		bool cheat_getInvincible();
		void cheat_toggleInvincible();

		bool cheat_getAiDisabled();
		void cheat_toggleAi();

		bool cheat_getHeightCheckDisabled();
		void cheat_toggleHeightCheck();
		
		bool cheat_getSmallMode();
		void cheat_toggleSmallMode();

		bool cheat_getData();
		void cheat_toggleData();	
		
		bool cheat_getFly();
		void cheat_toggleFly();
		
		bool cheat_getFullBright();
		void cheat_toggleFullBright();	
		
		bool cheat_getNoClip();
		void cheat_toggleNoClip();

		bool scriptRegister(asIScriptEngine* engine);
	};
}
#endif