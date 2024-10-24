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
	struct FloatResult {
		f32 value;
		bool pressed;

		FloatResult(const f32& value, bool pressed)
		{
			this->value = value;
			this->pressed = pressed;
		}
	};

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
		void SetWindowSize(f32 width, f32 height);
		void SetNextWindowPos(f32 width, f32 height);
		void SetNextWindowSize(f32 width, f32 height);
		void Begin(std::string& label, u32 flags);
		void End();
		//    - Layout
		void SameLine(f32 offsetX, f32 spacing = -1.0);
		void Separator();
		void SetNextItemWidth(f32 width);
		//    - Controls
		bool Button(std::string& label);
		bool Checkbox(std::string& label, bool* value);
		FloatResult SliderFloat(std::string& label, float* v, float v_min, float v_max, std::string& format, u32 flags = 0);
		// System
		bool scriptRegister(asIScriptEngine* engine);
	};
}
#endif