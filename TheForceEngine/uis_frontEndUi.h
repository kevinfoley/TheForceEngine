#pragma once
//////////////////////////////////////////////////////////////////////
// Support for scripting system to render GUI with AngelScript.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/system.h>
#ifdef ENABLE_FORCE_SCRIPT
#include <TFE_System/types.h>
#include <TFE_ForceScript/float2.h>
#include <TFE_FrontEndUI/frontEndUi.h>
#include <TFE_RenderBackend/renderBackend.h>
#include <string>

namespace TFE_FrontEndUI
{
	class UIS_FrontEndUi
	{
	public:
		f32 getUiScale();
		DisplayInfo getDisplayInfo();
		void Tooltip(std::string& text);
		// System
		bool scriptRegister(asIScriptEngine* engine);
	};
}
#endif