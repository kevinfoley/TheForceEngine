#pragma once
//////////////////////////////////////////////////////////////////////
// Support for scripting system to render GUI with AngelScript.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/system.h>
#ifdef ENABLE_FORCE_SCRIPT
#include <TFE_System/types.h>
#include <TFE_ForceScript/float2.h>
#include <TFE_FrontEndUI/frontEndUi.h>
#include <string>

namespace TFE_FrontEndUI
{
	class UIS_ImGui
	{
	public:
		// Properties
		// Functions
		//    - Text
		void Text(std::string& text);
		void TextWrapped(std::string& text);
		void LabelText(std::string& label, std::string& fmt);
		void PushFont(FontName fontName);
		void PopFont();
		//    - Window
		void SetNextWindowPos(f32 width, f32 height);
		void SetNextWindowSize(f32 width, f32 height);
		void Begin(std::string& label, u32 flags);
		void End();
		//    - Layout
		void Separator();
		//    - Controls
		bool Button(std::string& label);
		// System
		bool scriptRegister(asIScriptEngine* engine);
	};
}
#endif