#include "vueAsset.h"
#include <DXL2_System/system.h>
#include <DXL2_Asset/assetSystem.h>
#include <DXL2_Archive/archive.h>
#include <DXL2_System/parser.h>
#include <assert.h>
#include <map>
#include <algorithm>

namespace DXL2_VueAsset
{
	typedef std::map<std::string, VueAsset*> VueMap;
	static VueMap s_vueAssets;
	static std::vector<char> s_buffer;
	static const char* c_defaultGob = "DARK.GOB";

	bool parseVue(VueAsset* vue);

	VueAsset* get(const char* name)
	{
		VueMap::iterator iVue = s_vueAssets.find(name);
		if (iVue != s_vueAssets.end())
		{
			return iVue->second;
		}

		// It doesn't exist yet, try to load the font.
		if (!DXL2_AssetSystem::readAssetFromGob(c_defaultGob, name, s_buffer))
		{
			return nullptr;
		}

		VueAsset* vue = new VueAsset;
		if (!parseVue(vue))
		{
			delete vue;
			return nullptr;
		}

		s_vueAssets[name] = vue;
		strcpy(vue->name, name);
		return vue;
	}

	void freeAll()
	{
		VueMap::iterator iVue = s_vueAssets.begin();
		for (; iVue != s_vueAssets.end(); ++iVue)
		{
			VueAsset* vue = iVue->second;
			for (u32 i = 0; i < vue->transformCount; i++)
			{
				delete[] vue->transformNames[i];
			}
			delete[] vue->transformNames;
			delete[] vue->transforms;
			delete vue;
		}
		s_vueAssets.clear();
	}
		
	s32 getTransformIndex(const VueAsset* vue, const char* name)
	{
		if (!name || !vue) { return -1; }

		for (u32 i = 0; i < vue->transformCount; i++)
		{
			if (strcasecmp(name, vue->transformNames[i]) == 0)
			{
				return s32(i);
			}
		}
		return -1;
	}

	const VueTransform* getTransform(const VueAsset* vue, s32 transformIndex, s32 frameIndex)
	{
		if (!vue) { return nullptr; }
		return &vue->transforms[frameIndex * vue->transformCount + transformIndex];
	}

	const char* getTransformName(const VueAsset* vue, s32 transformIndex)
	{
		if (!vue) { return nullptr; }
		return vue->transformNames[transformIndex];
	}

	////////////////////////////////////////
	//////////// Internal //////////////////
	////////////////////////////////////////
	bool parseVue(VueAsset* vue)
	{
		if (s_buffer.empty()) { return false; }

		const size_t len = s_buffer.size();

		DXL2_Parser parser;
		size_t bufferPos = 0;
		parser.init(s_buffer.data(), len);
		parser.addCommentString("//");
		parser.addCommentString("#");
		parser.enableBlockComments();

		std::vector<std::string> transformNames;
		std::vector<VueTransform> transforms;

		s32 frameIndex = -1;
		u32 frameCount = 0;
		u32 transformIndex = 0;

		while (bufferPos < len)
		{
			const char* line = parser.readLine(bufferPos);
			if (!line) { break; }

			TokenList tokens;
			parser.tokenizeLine(line, tokens);
			if (tokens.size() < 1) { continue; }

			char* endPtr = nullptr;
			if (strcasecmp("frame", tokens[0].c_str()) == 0)
			{
				frameIndex++;
				s32 frameId = frameIndex;
				if (tokens.size() > 1u)
				{
					frameId = strtol(tokens[1].c_str(), &endPtr, 10);
				}
				// Are we allowed to skip frames?
				assert(frameId == frameIndex);
				transformIndex = 0;
				frameCount++;
			}
			else if (strcasecmp("transform", tokens[0].c_str()) == 0)
			{
				assert(frameIndex >= 0);
				assert(tokens.size() == 14u);
				const char* transformName = tokens[1].c_str();
				Mat3 rotScale = { 0 };
				for (u32 i = 0; i < 9; i++)
				{
					Vec3f& row = rotScale.m[i / 3];
					row.m[i%3] = (f32)strtod(tokens[2 + i].c_str(), &endPtr);
				}
				Vec3f translation = { 0 };
				for (u32 i = 0; i < 3; i++)
				{
					translation.m[i] = (f32)strtod(tokens[11 + i].c_str(), &endPtr);
				}
				translation.y = -translation.y;

				if (frameIndex == 0)
				{
					transformNames.push_back(transformName);
				}
				else
				{
					assert(strcasecmp(transformNames[transformIndex].c_str(), transformName) == 0);
				}
				transforms.push_back({ rotScale, translation });
				transformIndex++;
			}
		}

		// Allocate and fill in the data.
		vue->frameCount = frameCount;
		vue->transformCount = (u32)transformNames.size();
		vue->transformNames = new char*[vue->transformCount];
		vue->transforms = new VueTransform[vue->transformCount * vue->frameCount];
		assert(vue->transformCount * vue->frameCount == transforms.size());

		for (u32 i = 0; i < vue->transformCount; i++)
		{
			vue->transformNames[i] = new char[transformNames[i].size()];
			strcpy(vue->transformNames[i], transformNames[i].c_str());
		}
		memcpy(vue->transforms, transforms.data(), sizeof(VueTransform)*transforms.size());

		return true;
	}
}
