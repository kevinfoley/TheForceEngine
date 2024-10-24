#pragma once
#include <TFE_System/types.h>

namespace TFE_FrontEndUI
{
	void registerScriptFunctions();

	void executeLine(const char* line);
	void runUiScript(const char* scriptName);
	void showUiScript(const char* scriptName);

	void uiScript_update();
	void uiScript_updateConfigA11y(s32 tabWidth, u32 height);
	void uiScript_updateConfigDarkForcesCheats();
}