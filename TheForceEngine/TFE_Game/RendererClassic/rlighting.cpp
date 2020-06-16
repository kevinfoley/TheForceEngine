#include "rlighting.h"
#include "fixedPoint.h"
#include "rmath.h"
#include "rcommon.h"

using namespace RendererClassic;
using namespace RMath;

#if ENABLE_HIGH_PRECISION_FIXED_POINT == 0
#define LIGHT_SCALE 14
#define LIGHT_ATTEN0 20
#define LIGHT_ATTEN1 21
#else
#define LIGHT_SCALE 18ll
#define LIGHT_ATTEN0 24ll
#define LIGHT_ATTEN1 25ll
#endif

namespace RClassicLighting
{
	const u8* computeLighting(fixed16 depth, s32 lightOffset)
	{
		if (s_sectorAmbient >= 31)
		{
			return nullptr;
		}
		depth = max(depth, fixed16(0));
		s32 light = 0;

		// handle camera lightsource
		if (s_worldAmbient < 31 && s_cameraLightSource != 0)
		{
			s32 depthScaled = min(s32(depth >> LIGHT_SCALE), 127);
			s32 lightSource = 31 - (s_lightSourceRamp[depthScaled] + s_worldAmbient);
			if (lightSource > 0)
			{
				light += lightSource;
			}
		}

		s32 secAmb = s_sectorAmbient;
		if (light < secAmb) { light = secAmb; }

		// depthScale = 0.09375 (3/32)
		// light = max(light - depth * depthScale, secAmb*0.875)
		s32 depthAtten = s32((depth >> LIGHT_ATTEN0) + (depth >> LIGHT_ATTEN1));		// depth/16 + depth/32
		light = max(light - depthAtten, s_scaledAmbient);

		if (lightOffset != 0)
		{
			light += lightOffset;
		}
		if (light >= 31) { return nullptr; }

		return &s_colorMap[light << 8];
	}
}