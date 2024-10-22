#pragma once
#include <TFE_System/types.h>

namespace TFE_FrontEndUI
{
	void registerScriptFunctions();

	void executeLine(const char* line);
	void runUiScript(const char* scriptName);
	void showUiScript(const char* scriptName);

	void uiScript_update();
}
