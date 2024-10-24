#include "UIS_Imgui.h"
#include <TFE_System/system.h>
#include <TFE_ForceScript/float2.h>
#include <TFE_FrontEndUI/frontEndUi.h>
#include <TFE_Ui/ui.h>


#ifdef ENABLE_FORCE_SCRIPT
#include <angelscript.h>
namespace TFE_FrontEndUI
{
	////////////////////////////////////////////////////////
	//  Functions
	////////////////////////////////////////////////////////
	// Text
	void UIS_ImGui::Text(std::string& text)
	{
		ImGui::Text(text.c_str());
	}
	void UIS_ImGui::TextWrapped(std::string& text)
	{
		ImGui::TextWrapped(text.c_str());
	}
	void UIS_ImGui::LabelText(std::string& label, std::string& fmt)
	{
		ImGui::LabelText(label.c_str(), fmt.c_str());
	}
	void UIS_ImGui::PushFont(FontName fontName)
	{
		ImFont* font = getFont(fontName);
		ImGui::PushFont(font);
	}
	void UIS_ImGui::PopFont()
	{
		ImGui::PopFont();
	}

	// Window
	void UIS_ImGui::SetWindowSize(f32 width, f32 height)
	{
		ImGui::SetWindowSize(ImVec2(width, height));
	}
	void UIS_ImGui::SetNextWindowPos(f32 width, f32 height)
	{
		ImGui::SetNextWindowPos(ImVec2(width, height));
	}
	void UIS_ImGui::SetNextWindowSize(f32 width, f32 height)
	{
		ImGui::SetNextWindowSize(ImVec2(width, height));
	}
	void UIS_ImGui::Begin(std::string& label, u32 flags)
	{
		ImGui::Begin(label.c_str(), nullptr, flags);
	}
	void UIS_ImGui::End()
	{
		ImGui::End();
	}
	
	// Layout
	void UIS_ImGui::SameLine(f32 offsetX, f32 spacing)
	{
		ImGui::SameLine(offsetX, spacing);
	}

	void UIS_ImGui::Separator()
	{
		ImGui::Separator();
	}

	void UIS_ImGui::SetNextItemWidth(f32 width)
	{
		ImGui::SetNextItemWidth(width);
	}

	// Controls
	bool UIS_ImGui::Button(std::string& label)
	{
		return ImGui::Button(label.c_str());
	}
	bool UIS_ImGui::Checkbox(std::string& label, bool* value)
	{
		return ImGui::Checkbox(label.c_str(), value);
	}
	FloatResult UIS_ImGui::SliderFloat(std::string& label, float* v, float v_min, float v_max, std::string& format, u32 flags)
	{
		bool pressed;
		if (format.empty()) { pressed = ImGui::SliderFloat(label.c_str(), v, v_min, v_max, "%.3f", flags); }
		else { pressed = ImGui::SliderFloat(label.c_str(), v, v_min, v_max, format.c_str(), flags); }
		return FloatResult(*v, pressed);
	}

	////////////////////////////////////////////////////////
	//  System
	////////////////////////////////////////////////////////
	bool UIS_ImGui::scriptRegister(asIScriptEngine* engine)
	{
		s32 res = 0;

		////////////////////////////////////////////////////////
		//  Enums
		////////////////////////////////////////////////////////
		// FontName:
		res = engine->RegisterEnum("FontName"); assert(res >= 0);
		res = engine->RegisterEnumValue("FontName", "FONT_DIALOG", FONT_DIALOG); assert(res >= 0);
		res = engine->RegisterEnumValue("FontName", "FONT_MENU", FONT_MENU); assert(res >= 0);
		res = engine->RegisterEnumValue("FontName", "FONT_TITLE", FONT_TITLE); assert(res >= 0);
		res = engine->RegisterEnumValue("FontName", "FONT_VERSION", FONT_VERSION); assert(res >= 0);


		////////////////////////////////////////////////////////
		//  Objects
		////////////////////////////////////////////////////////
		// FloatResult:
		res = engine->RegisterObjectType("FloatResult", sizeof(FloatResult), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<TFE_FrontEndUI::FloatResult>()); assert(res >= 0);
		res = engine->RegisterObjectProperty("FloatResult", "float value", asOFFSET(FloatResult, value)); assert(res >= 0);
		res = engine->RegisterObjectProperty("FloatResult", "bool pressed", asOFFSET(FloatResult, pressed)); assert(res >= 0);

		// This helper class:
		res = engine->RegisterObjectType("Imgui", sizeof(UIS_ImGui), asOBJ_VALUE | asOBJ_POD); assert(res >= 0);
		//   - Text
		res = engine->RegisterObjectMethod("Imgui", "void Text(const string &in)", asMETHOD(UIS_ImGui, Text), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void TextWrapped(const string &in)", asMETHOD(UIS_ImGui, TextWrapped), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void LabelText(const string &in, const string &in)", asMETHOD(UIS_ImGui, LabelText), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void PushFont(FontName)", asMETHOD(UIS_ImGui, PushFont), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void PopFont()", asMETHOD(UIS_ImGui, PopFont), asCALL_THISCALL);  assert(res >= 0);
		//   - Window
		res = engine->RegisterObjectMethod("Imgui", "void SetWindowSize(float, float)", asMETHOD(UIS_ImGui, SetWindowSize), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void SetNextWindowPos(float, float)", asMETHOD(UIS_ImGui, SetNextWindowPos), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void SetNextWindowSize(float, float)", asMETHOD(UIS_ImGui, SetNextWindowSize), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void Begin(const string &in, uint)", asMETHOD(UIS_ImGui, Begin), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void End()", asMETHOD(UIS_ImGui, End), asCALL_THISCALL);  assert(res >= 0);
		//   - Layout
		res = engine->RegisterObjectMethod("Imgui", "void SameLine()", asMETHOD(UIS_ImGui, SameLine), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void Separator()", asMETHOD(UIS_ImGui, Separator), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "void SetNextItemWidth()", asMETHOD(UIS_ImGui, SetNextItemWidth), asCALL_THISCALL);  assert(res >= 0);
		//   - Controls
		res = engine->RegisterObjectMethod("Imgui", "bool Button(const string &in)", asMETHOD(UIS_ImGui, Button), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "bool Checkbox(const string &in, bool&in)", asMETHOD(UIS_ImGui, Checkbox), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("Imgui", "FloatResult SliderFloat(const string &in, float&in, float, float, const string &in, uint)", asMETHOD(UIS_ImGui, SliderFloat), asCALL_THISCALL);  assert(res >= 0);
		//   - Script variable
		res = engine->RegisterGlobalProperty("Imgui imgui", this);  assert(res >= 0);
		return res >= 0;
	}
}
#endif