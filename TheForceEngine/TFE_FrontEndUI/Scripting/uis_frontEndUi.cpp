#include "UIS_FrontEndUi.h"
#include <TFE_System/system.h>
#include <TFE_ForceScript/float2.h>
#include <TFE_FrontEndUI/frontEndUi.h>
#include <TFE_RenderBackend/renderBackend.h>
#include <TFE_Ui/ui.h>


#ifdef ENABLE_FORCE_SCRIPT
#include <angelscript.h>
namespace TFE_FrontEndUI
{
	////////////////////////////////////////////////////////
	//  Functions
	////////////////////////////////////////////////////////
	f32 UIS_FrontEndUi::getUiScale() { return TFE_FrontEndUI::getUiScale(); }
	DisplayInfo UIS_FrontEndUi::getDisplayInfo()
	{
		DisplayInfo display;
		TFE_RenderBackend::getDisplayInfo(&display);
		return display;
	}

	void UIS_FrontEndUi::Tooltip(std::string& text)
	{
		TFE_FrontEndUI::Tooltip(text.c_str());
	}

	////////////////////////////////////////////////////////
	//  System
	////////////////////////////////////////////////////////
	bool UIS_FrontEndUi::scriptRegister(asIScriptEngine* engine)
	{
		s32 res = 0;

		////////////////////////////////////////////////////////
		//  OBJECTS 
		////////////////////////////////////////////////////////
		// DisplayInfo:
		res = engine->RegisterObjectType("DisplayInfo", sizeof(DisplayInfo), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<DisplayInfo>()); assert(res >= 0);
		res = engine->RegisterObjectProperty("DisplayInfo", "uint width", asOFFSET(DisplayInfo, width)); assert(res >= 0);
		res = engine->RegisterObjectProperty("DisplayInfo", "uint height", asOFFSET(DisplayInfo, height)); assert(res >= 0);
		res = engine->RegisterObjectProperty("DisplayInfo", "float refreshRate", asOFFSET(DisplayInfo, refreshRate)); assert(res >= 0);

		// This helper class:
		res = engine->RegisterObjectType("FrontEndUi", sizeof(UIS_FrontEndUi), asOBJ_VALUE | asOBJ_POD); assert(res >= 0);
		res = engine->RegisterObjectMethod("FrontEndUi", "float getUiScale()", asMETHOD(UIS_FrontEndUi, getUiScale), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("FrontEndUi", "DisplayInfo getDisplayInfo()", asMETHOD(UIS_FrontEndUi, getDisplayInfo), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterObjectMethod("FrontEndUi", "void Tooltip(const string &in)", asMETHOD(UIS_FrontEndUi, Tooltip), asCALL_THISCALL);  assert(res >= 0);
		res = engine->RegisterGlobalProperty("FrontEndUi frontEndUi", this);  assert(res >= 0);
		return res >= 0;
	}
}
#endif