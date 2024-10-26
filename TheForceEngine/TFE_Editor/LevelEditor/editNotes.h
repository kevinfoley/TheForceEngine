#pragma once
//////////////////////////////////////////////////////////////////////
// The Force Engine Editor
// A system built to view and edit Dark Forces data files.
// The viewing aspect needs to be put in place at the beginning
// in order to properly test elements in isolation without having
// to "play" the game as intended.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include "levelEditorData.h"
#include "note.h"
#include <vector>
#include <string>

namespace LevelEditor
{
	void handleLevelNoteEdit(RayHitInfo* hitInfo, const Vec3f& rayDir);
}
