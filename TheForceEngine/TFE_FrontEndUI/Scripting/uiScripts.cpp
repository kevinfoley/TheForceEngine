#include "uiScripts.h"
#include <TFE_System/math.h>
#include <TFE_System/system.h>
#include <TFE_Jedi/Math/core_math.h>
#include <TFE_Editor/editor.h>
#include <TFE_Editor/errorMessages.h>
#include <TFE_ForceScript/forceScript.h>
#include <TFE_Editor/LevelEditor/infoPanel.h>
#include <TFE_Editor/EditorAsset/editorTexture.h>
#include <TFE_Editor/EditorAsset/editorFrame.h>
#include <TFE_Editor/EditorAsset/editorSprite.h>
#include <TFE_Ui/ui.h>

#include <algorithm>
#include <vector>
#include <string>
#include <map>

#ifdef ENABLE_FORCE_SCRIPT
#include <angelscript.h>
#include <uis_darkForces.h>
#include <uis_Imgui.h>
#include <uis_FrontEndUi.h>
#include <uis_A11y.h>

namespace TFE_FrontEndUI
{
	const bool AUTO_RELOAD_SCRIPTS = false;
	static char s_scriptBuffer[4096];
	static std::vector<std::string> s_scriptsToRun;

	bool s_levelScriptRegistered = false;
	bool s_execFromOutput = false;
	UIS_DarkForces s_uisDarkForces;
	UIS_ImGui s_uisImgui;
	UIS_FrontEndUi s_uisFrontEndUi;
	UIS_A11y s_uisA11y;
	std::map<std::string, TFE_ForceScript::ModuleHandle> s_modules;

	// Print any script messages, warnings or errors to the editor output.
	void scriptCallback(LogWriteType type, const char* section, s32 row, s32 col, const char* msg)
	{
		
	}

	void registerScriptFunctions()
	{
		if (s_levelScriptRegistered) { return; }
		s_levelScriptRegistered = true;
		TFE_ForceScript::overrideCallback(scriptCallback);

		asIScriptEngine* engine = (asIScriptEngine*)TFE_ForceScript::getEngine();
		s_uisDarkForces.scriptRegister(engine);
		s_uisImgui.scriptRegister(engine);
		s_uisFrontEndUi.scriptRegister(engine);
		s_uisA11y.scriptRegister(engine);

		// Math/Intrinsics.
	}

	void executeLine(const char* line)
	{
		s_execFromOutput = true;

		// Short-cuts to make running script commands on the console easier.
		strcpy(s_scriptBuffer, "#include \"EditorDef/Scripts/scriptConsole.h\"");
		// Wrap the console line in a function.
		// Add an extra ';' at the end so it isn't required when typing in the console.
		// (If one is typed, you get ;; which will still compile fine)
		strcat(s_scriptBuffer, "void main()\n{\n");
		strcat(s_scriptBuffer, line);
		strcat(s_scriptBuffer, ";}\n");

		s_execFromOutput = false;
	}

	void runUiScript(const char* scriptName)
	{
		s_scriptsToRun.push_back(scriptName);
	}

	void showUiScript(const char* scriptName)
	{
		char scriptPath[TFE_MAX_PATH];
		sprintf(scriptPath, "EditorDef/Scripts/%s.fs", scriptName);
		FileStream file;
		if (file.open(scriptPath, Stream::MODE_READ))
		{
			std::vector<char> text;
			const size_t size = file.getSize();
			text.resize(size + 1);
			file.readBuffer(text.data(), (u32)size);
			text.data()[size] = 0;
			file.close();

			char title[256];
			sprintf(title, "Script - %s", scriptName);
			TFE_Editor::showMessageBox(title, "%s", text.data());
		}
	}

	void uiScript_update()
	{
		char scriptPath[TFE_MAX_PATH];
		const s32 count = (s32)s_scriptsToRun.size();
		const std::string* scriptName = s_scriptsToRun.data();
		for (s32 i = 0; i < count; i++, scriptName++)
		{
			sprintf(scriptPath, "EditorDef/Scripts/%s.fs", scriptName->c_str());

			if (AUTO_RELOAD_SCRIPTS || !s_modules.count(*scriptName)) {
				s_modules[*scriptName] = TFE_ForceScript::createModule(scriptName->c_str(), scriptPath);
			}
			if (s_modules.count(*scriptName))
			{
				TFE_ForceScript::ModuleHandle handle = s_modules[*scriptName];
				TFE_ForceScript::FunctionHandle func = TFE_ForceScript::findScriptFunc(handle, "void main()");
				TFE_ForceScript::execFunc(func);
			}
		}
		s_scriptsToRun.clear();
	}
	
	void uiScript_updateConfigA11y(s32 tabWidth, u32 height)
	{
		s_scriptsToRun.push_back("ConfigA11y");
		uiScript_update();
	}
	
	void uiScript_updateConfigDarkForcesCheats()
	{
		s_scriptsToRun.push_back("ConfigCheats");
		uiScript_update();
	}
}
#else
namespace TFE_FrontEndUI
{
	void registerScriptFunctions() {}

	void executeLine(const char* line) {}
	void runUiScript(const char* scriptName) {}
	void showUiScript(const char* scriptName) {}

	void uiScript_update() {}
	void uiScript_updateConfigA11y() {}
	void uiScript_updateConfigDarkForcesCheats() {}
}
#endif
